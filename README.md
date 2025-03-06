# <p align = "center">Custom Interpreter from scratch</p>
This is a custom interpreter implemented from scratch using C++.

## Setup
Add the folder to the system path to allow the interpreter to work anywhere in the system.

create a "test.txt" file and write
```
print("hello world");
```
execute the file by executing in the command prompt
```
interpreter test.txt
```

## Documentation
### general
The program execution starts from the first line of the program and proceeds in order till the end of the file.

### variables 
variables can only hold integer values.
Any variable declared has a global scope.
They can be declared as follows
```
a = 1;
b = 2 + a;
c = a*b + (2/3);
```

### predefined functions
#### print 
use "print" to output anything to the console.
```
print("hello world");
print("value of a : ", a);
```

#### input
use "input" fetch data from the user.
"input" only accepts integers.
it can be used as follows:
```
a = input("enter value of a: ");
```

### conditionals
It supports if-else-if ladder like any other language.
sample program to check if leap year or not using if-else-if ladder.
```
year = input("enter year")
if(year % 4 == 0){
	if(year % 100 == 0 and year % 400 != 0){
		print("not a leap year");
	}
	else{
		print("leap year");
	}
}
else{
	print("not a leap year");
}
```
### loops
It supports while loops like any other language.
sample program to print numbers from 1 to 10.
```
a = 1;
while(a <= 10){
	print(a);
	a = a+1;
}
```
#### loop controls
##### break
stops further execution of the loop.
```
a = 1;
while(a <= 5){
	if(a == 3){
		break;
	}
	print(a, "\n");
	a=a+1;
}
```
output of the above code:
```
1
2
 ```
##### continue
stops execution of the current iteration of the loop and proceeds to the next iteration.
```
a = 1;
while(a <= 5){
	if(a == 3){
		a=a+1;
		continue;
	}
	print(a, "\n");
	a=a+1;
}
```
output of the above code:
```
1
2
4
5
 ```

## Future development scope
1. allow creation of string variables
2. allow input of strings
3. implementation basic string related functions