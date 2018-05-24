MAIN_EXECUTABLE = /sbin/locktones
SERVICE_UNIT = /etc/systemd/system/locktones.service
WRAPPER_SCRIPT = /usr/libexec/locktones-wrapper
CONFIGURATION_FILE = /etc/sysconfig/locktones

systemd-install:
	install -m 755 ../locktones $(MAIN_EXECUTABLE)
	install -m 644 systemd.service $(SERVICE_UNIT)
	install -m 755 systemd.wrapper $(WRAPPER_SCRIPT)
	install -m 644 config $(CONFIGURATION_FILE)

systemd-uninstall:
	rm -f $(MAIN_EXECUTABLE)
	rm -f $(SERVICE_UNIT)
	rm -f $(WRAPPER_SCRIPT)
	rm -f $(CONFIGURATION_FILE)

