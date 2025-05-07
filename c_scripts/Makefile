all: 
	gcc -g -I/opt/homebrew/include -L/opt/homebrew/lib login.c -o login.cgi -lmysqlclient -lssl -lcrypto
	gcc -g -I/opt/homebrew/include -L/opt/homebrew/lib product.c -o product.cgi -lmysqlclient -lssl -lcrypto
	gcc -g -I/opt/homebrew/include -L/opt/homebrew/lib update_stock.c -o update_stock.cgi -lmysqlclient -lssl -lcrypto
	gcc -g -I/opt/homebrew/include -L/opt/homebrew/lib orders.c -o orders.cgi -lmysqlclient -lssl -lcrypto

move:
	sudo cp login.cgi /opt/homebrew/var/www/cgi-bin/os_mini/login.cgi
	sudo cp product.cgi /opt/homebrew/var/www/cgi-bin/os_mini/product.cgi
	sudo cp update_stock.cgi /opt/homebrew/var/www/cgi-bin/os_mini/update_stock.cgi
	sudo cp orders.cgi /opt/homebrew/var/www/cgi-bin/os_mini/orders.cgi
	chmod +x /opt/homebrew/var/www/cgi-bin/os_mini/*.cgi
