all:
	+$(MAKE) -C Server
	+$(MAKE) -C Client
	echo "Done. call './server PORT' to open the server and './client HOST_IP PORT' to conect the client."