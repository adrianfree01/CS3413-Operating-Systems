all: build copy 1 2 3 4 5 6

build:
	gcc -g ./code/*.c
copy:
	cp -r ./tests ./recovered
# for testing the time to reverse
1:
	./a.out ./recovered/test1.png > recovered/output1.txt
2:
	./a.out ./recovered/test2.png > recovered/output2.txt
3:
	./a.out ./recovered/test3.png > recovered/output3.txt
4:
	./a.out ./recovered/test4.png > recovered/output4.txt
5:
	./a.out ./recovered/test5.png > recovered/output5.txt
6:
	./a.out ./recovered/test6.png > recovered/output6.txt
# testing when services come in some order
clean:
	rm  -f ./recovered/* ./a.out 
