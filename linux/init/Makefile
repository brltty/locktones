MAIN_EXECUTABLE = /sbin/locktones
CONFIGURATION_FILE = /etc/sysconfig/locktones
WRAPPER_SCRIPT = /usr/libexec/locktones-wrapper
SERVICE_UNIT = /etc/systemd/system/locktones.service

systemd-install:
	install -m 755 ../locktones $(MAIN_EXECUTABLE)
	install -m 644 config $(CONFIGURATION_FILE)
	install -m 755 systemd.wrapper $(WRAPPER_SCRIPT)
	install -m 644 systemd.service $(SERVICE_UNIT)

systemd-uninstall:
	rm -f $(MAIN_EXECUTABLE)
	rm -f $(CONFIGURATION_FILE)
	rm -f $(WRAPPER_SCRIPT)
	rm -f $(SERVICE_UNIT)

