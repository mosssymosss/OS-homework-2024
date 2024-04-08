run: init client destroy
	./init
	./client
	./destroy

init: bank.h init.cpp bank.o bank_cell.o
	g++ init.cpp bank.o bank_cell.o -o init -lrt

client: bank.h client.cpp bank.o bank_cell.o
	g++ client.cpp bank.o bank_cell.o -o client -lrt

destroy: bank.h destroy.cpp bank.o bank_cell.o
	g++ destroy.cpp bank.o bank_cell.o -o destroy -lrt

bank_cell.o: bank_cell.cpp bank.h
	g++ bank_cell.cpp -c

bank.o: bank_cell.h bank.h bank.cpp
	g++ bank.cpp -c

clean:
	rm -f init client destroy *.o