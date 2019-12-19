rm main > /dev/null 2> /dev/null;
clear;

make;

FILE=./main;
if [ -f "$FILE" ]; then
	sudo ./main;
fi
