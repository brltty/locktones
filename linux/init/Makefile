PROGRAM_NAME = locktones
PROGRAM_EXECUTABLE = /sbin/$(PROGRAM_NAME)

install-executable:
	install -m 755 ../$(PROGRAM_NAME) $(PROGRAM_EXECUTABLE)

uninstall-executable:
	rm -f $(PROGRAM_EXECUTABLE)

SYSTEMD_CONFIG = /etc/sysconfig/$(PROGRAM_NAME)
SYSTEMD_WRAPPER = /usr/libexec/$(PROGRAM_NAME)-wrapper
SYSTEMD_UNIT = /etc/systemd/system/$(PROGRAM_NAME).service

install-systemd: install-executable
	install -m 644 config $(SYSTEMD_CONFIG)
	install -m 755 systemd.wrapper $(SYSTEMD_WRAPPER)
	install -m 644 systemd.service $(SYSTEMD_UNIT)

uninstall-systemd: uninstall-executable
	rm -f $(SYSTEMD_CONFIG)
	rm -f $(SYSTEMD_WRAPPER)
	rm -f $(SYSTEMD_UNIT)

reload-systemd:
	systemctl daemon-reload

