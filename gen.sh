#!/bin/bash

# PCANNON GEN.SH v1.1S - FROM PCANNON PROJECT STANDARDS
# STANDARD: 20260714
# https://github.com/pcannon09/pcannonProjectStandards

source ./utils/inc/sh/colors.sh

DIR_GEN=(
		".private"
		".private/dev"
)

FILE_GEN=(
		".root_dir"
		".private/dev/compilation.json"
)

PROJECT_INFO_PATH=".private/project.json"
GENERATED=0

projectName=$(jq -r '.exeName' "$PROJECT_INFO_PATH")

if [ "$1" == "link" ]; then
	readonly DEST="$PWD/run.sh"

	if [ -f "$DEST" ]; then
		rm -vf "$DEST"
	fi

	if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
		execName="${projectName}_exec.exe"
	else
		execName="${projectName}_exec"
	fi

	if [ "$2" == "root" ]; then
		ln -vs "./build/$execName" "$DEST"
	else
		ln -vs "./build/bin/$execName" "$DEST"
	fi

	GENERATED=1

elif [ "$1" == "ungen" ]; then
	echo -e "${BOLD}[ * ] Removing generated files"
	echo -e "${RED}[ WARN ] Are you sure that you want to continue? This will remove all existing data [ Y / N / GET ]${RESET}"

	read answer

	if [ "${answer,,}" == "yes" ] || [ "${answer,,}" == "y" ]; then
		printf "[ DEL ] Deleting all from \`.private/\` " ; find .private -mindepth 1 ! -name "project.json" -delete ; echo "[ DONE ]"

		echo -e "[ DONE ]${RESET}"

	elif [ "${answer,,}" == "get" ]; then
		tree .private/

		echo "[ DONE ]"

	else
		echo -e "${RESET}Abort."
	fi

	echo -e "${BOLD}${CYAN}[ NOTE ] Execute this file without the \`ungen\` flag to generate all files"
	exit

elif [ "$1" == "doxygen" ]; then
	doxygen Doxyfile
fi

for dir in "${DIR_GEN[@]}"; do
	if [ -d "$dir" ]; then
		continue
	fi

	printf "${BOLD}${BRIGHT_GREEN}[ MKDIR ] Generating \`$dir\` directory${RESET} " ; mkdir -p $dir ; echo -e "${BOLD}${BRIGHT_GREEN}[ OK ]${RESET}"

	GENERATED=1
done

for file in "${FILE_GEN[@]}"; do
	if [ -f "$file" ]; then
		continue
	fi

	printf "${BOLD}${BRIGHT_GREEN}[ TOUCH ] Generating \`$file\` file${RESET} " ; touch $file ; echo -e "${BOLD}${BRIGHT_GREEN}[ OK ]${RESET}"
	GENERATED=1
done

if [ -f "Doxyfile" ] && [ "$1" == "doxy" ]; then
	doxygen Doxyfile
fi

if [ $GENERATED -eq 1 ]; then
	echo -e "[ DONE ]"

elif [ $GENERATED -eq 0 ]; then
	echo -e "[ DONE ] No files to generate"
fi

