server:
	gcc server.c user_commands.c admin_commands.c -lpthread -o server
 
client:
	gcc client.c -o client

clean:
	rm client server
