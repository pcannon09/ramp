#!/bin/bash

# PCANNON COMPILE.SH v1.3 - FROM PCANNON PROJECT STANDARDS
# STANDARD: To Be Approved
# https://github.com/pcannon09/pcannonProjectStandards

set -e

# ROOT CHECK

if [[ ! -f ".root_dir" ]]; then
	echo "Need a \`.root_dir\` file for indication"
	echo "Execute \`gen.sh\` at the repo root"
	exit 1
fi

# IMPORTS

source "./utils/inc/sh/colors.sh"

# CONSTANTS

readonly COMPILATION_FILE_PATH=".private/dev/compilation.json"
readonly PROJECT_INFO_PATH=".private/project.json"
readonly BUILD_DIR="./build"
readonly BACKUP_DIR="./build/bin/exeBackup"

readonly DEFAULT_BUILD_TYPE="Debug"

export NINJA_STATUS="${BOLD}${BRIGHT_BLACK}[ ${GREEN}%p${BRIGHT_BLACK} :: ${BLUE}%r running${BRIGHT_BLACK} :: ${RED}%u/%t${BRIGHT_BLACK} :: ${MAGENTA}%o/s${BRIGHT_BLACK} :: ${CYAN}%e${BRIGHT_BLACK} ] ${RESET}"

# GLOBAL STATE

CHECKS=true
BUILD_TYPE="$DEFAULT_BUILD_TYPE"

cores=""
enableBackup=""
projectName=""
compilerFlags=""

declare -a compileMacros=()
declare -a cmakeCommand=()

# UTILITY FUNCTIONS

__printError() {
	echo -e "${BRIGHT_RED}[ ERROR ] $*${RESET}"
}

__printInfo() {
	echo -e "${BRIGHT_BLUE}${BOLD}[ INFO ] $*${RESET}"
}

__printWarning() {
	echo -e "${BRIGHT_YELLOW}${BOLD}[ WARN ] $*${RESET}"
}

__commandExists() {
	command -v "$1" > /dev/null 2>&1
}

# ARGUMENT HANDLING

__parseArguments() {
	if [[ "$1" == "nochecks" || "$2" == "nochecks" || "$3" == "nochecks" ]]; then
		CHECKS=false
	fi

	if [[ "$2" == "ndev" ]]; then
		BUILD_TYPE="Release"
	fi
}

# DEPENDENCY CHECKS

__checkDependencies() {
	[[ "$CHECKS" == true ]] || return 0

	if ! __commandExists cmake; then
		__printError "Please have 'cmake' installed"
		exit 1
	fi

	if ! __commandExists ninja && ! __commandExists make; then
		__printError "Please have 'ninja' or 'make' installed"
		exit 1
	fi

	if ! __commandExists jq; then
		__printError "Please have 'jq' installed"
		exit 1
	fi
}

# CONFIGURATION

__loadConfiguration() {
	cores=$(jq '.cores' "$COMPILATION_FILE_PATH")
	enableBackup=$(jq -r '.enableBackup' "$COMPILATION_FILE_PATH")
	projectName=$(jq -r '.exeName' "$PROJECT_INFO_PATH")

	mapfile -t compileMacros < <(
		jq -r '.macros // [] | .[]' "$COMPILATION_FILE_PATH"
	)
}

# BUILD DIRECTORY

__ensureBuildDirectory() {
	mkdir -p "$BUILD_DIR"
}

# BACKUP

__backupExecutable() {
	case "$enableBackup" in
		YES|yes|y)
			;;
		*)
			return 0
			;;
	esac

	printf "${BOLD}${GREEN}[ INFO ] Backing up executable\n${RESET}"

	mkdir -p "$BACKUP_DIR"

	if [[ -f "$BUILD_DIR/bin/$projectName" ]]; then
		cp \
			"$BUILD_DIR/bin/$projectName" \
			"$BACKUP_DIR/$projectName-$(date +%s)"
	fi

	echo -e " [ Done ]"
}

# BUILD

__compileSoftware() {
	cmake --build "$BUILD_DIR" -j"$cores" -v
}

# CMAKE MACROS

__buildCompilerFlags() {
	compilerFlags=""

	for macro in "${compileMacros[@]}"; do
		if [[ -n "$macro" ]]; then
			compilerFlags+=" -D$macro"
		else
			__printWarning "Skipping empty macro \`$macro\`"
		fi
	done
}

# CMAKE SETUP

__setupCMake() {
	local systemName=""

	if [[ "$2" != "ndev" ]]; then
		systemName="$2"
		shift
	elif [[ -n "$3" ]]; then
		systemName="$3"
		shift
	fi

	__buildCompilerFlags

	cmakeCommand=(
		cmake
		-S ..
		-B .
		-G Ninja
		-DCMAKE_BUILD_TYPE="$BUILD_TYPE"
		-DCMAKE_POLICY_VERSION_MINIMUM=3.5
		-DCMAKE_C_FLAGS="$compilerFlags"
	)

	if [[ "$systemName" == "tests" ]]; then
		cmakeCommand+=(
				-DPYAQ_BUILD_TESTS=ON
		)
	fi

	__printInfo "Running CMake:"
	printf ' %q' "${cmakeCommand[@]}"
	echo

	local previousDirectory
	previousDirectory=$(pwd)

	cd "$BUILD_DIR"
	"${cmakeCommand[@]}"
	cd "$previousDirectory"
}

# SETTINGS

__createCompilationConfig() {
	if [[ ! -s "$COMPILATION_FILE_PATH" ]]; then
		mkdir -p "$(dirname "$COMPILATION_FILE_PATH")"

		cat > "$COMPILATION_FILE_PATH" <<EOF
{
"cores": 10,
"enableBackup": "NO",
"macros": []
}
EOF
	fi
}

__readCoreSetting() {
	echo -e "${BOLD}[ PROMPT ] Enter the number of cores to compile the program ('same' to keep current)${RESET}"
	read -r cores

	case "$cores" in
		same)
			cores=$(jq '.cores' "$COMPILATION_FILE_PATH")
			echo -e "${GREEN}$cores${RESET}"
			;;

		[0-9]*)
			if [[ ! "$cores" =~ ^[0-9]+$ ]]; then
				__printError "Give a number for \`cores\`, not a string or boolean"
				exit 1
			fi
			;;

		*)
			__printError "Give a number for \`cores\`, not a string or boolean"
			exit 1
			;;
	esac
}

__readBackupSetting() {
	echo -e "${BOLD}[ PROMPT ] Enable backup? 'YES' or 'NO' ('same' to keep current)${RESET}"
	read -r enableBackup

	if [[ "$enableBackup" == "same" ]]; then
		enableBackup=$(jq -r '.enableBackup' "$COMPILATION_FILE_PATH")
		echo -e "${GREEN}$enableBackup${RESET}"
	fi
}

__readMacrosSetting() {
	local macroVal

	declare -a programMacros=()

	echo -e "${BOLD}[ PROMPT ] Macros (OPTIONAL)\n(done: Stop adding)\n(clear: Clear list)\n(same: Keep current list)${RESET}"

	while IFS= read -r macroVal; do
		case "$macroVal" in
			same)
				mapfile -t programMacros < <(
					jq -r '.macros[]' "$COMPILATION_FILE_PATH"
				)

				echo "[ CURRENT MACROS ]"
				printf "${GREEN} - %s\n" "${programMacros[@]}"
				printf "${RESET}"

				break
				;;

			done)
				break
				;;

			clear)
				programMacros=()
				echo "[ CLEARED ]"
				;;

			*)
				programMacros+=("$macroVal")
				;;
		esac
	done

	echo -e "${BRIGHT_BLUE}${BOLD}[ * ] TOTAL MACROS [${programMacros[*]}]${RESET}"

	PROGRAM_MACROS=("${programMacros[@]}")
}

__writeCompilationConfig() {
	local macrosJson

	macrosJson=$(
		printf '%s\n' "${PROGRAM_MACROS[@]}" |
			jq -R . |
			jq -s .
	)

	mkdir -p tmp

	jq \
		--argjson cores "$cores" \
		--arg enableBackup "$enableBackup" \
		--argjson macros "$macrosJson" \
		'
		.cores = $cores
		| .enableBackup = $enableBackup
		| .macros = $macros
		' \
		"$COMPILATION_FILE_PATH" > tmp/tmp_dev_compilation.json

	mv \
		tmp/tmp_dev_compilation.json \
		"$COMPILATION_FILE_PATH"
}

__settings() {
	echo -e "[ * ] Compilation settings"

	mkdir -p tmp

	__createCompilationConfig
	__readCoreSetting
	__readBackupSetting
	__readMacrosSetting
	__writeCompilationConfig

	echo -e "[ DONE ]"
}

# MAIN

__parseArguments "$@"
__checkDependencies
__loadConfiguration
__ensureBuildDirectory
__backupExecutable

case "$1" in
	setup)
		__setupCMake "$@"
		;;

	settings)
		__settings
		;;

	""|m|nochecks)
		__compileSoftware "$@"
		;;

	*)
		__printError "Unknown command: $1"
		exit 1
		;;
esac

