all:
	+$(MAKE) -C Server
	+$(MAKE) -C Client
	echo "Done. call './Server/server PORT' to open the server and './Client/client HOST_IP PORT' to conect the client."