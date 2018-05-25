INSTALL = install
INSTALL_DATA = $(INSTALL) -m 644
INSTALL_PROGRAM = $(INSTALL) -m 755
INSTALL_DIRECTORY = $(INSTALL) -m 755 -d

PROGRAM_NAME = locktones
PROGRAM_EXECUTABLE = /sbin/$(PROGRAM_NAME)

install-executable:
	$(INSTALL_PROGRAM) ../$(PROGRAM_NAME) $(PROGRAM_EXECUTABLE)

uninstall-executable:
	-rm -f $(PROGRAM_EXECUTABLE)

SYSTEMD_CONFIG = /etc/sysconfig/$(PROGRAM_NAME)
SYSTEMD_WRAPPER = /usr/libexec/$(PROGRAM_NAME)-wrapper
SYSTEMD_UNIT = /etc/systemd/system/$(PROGRAM_NAME).service

install-systemd: install-executable
	$(INSTALL_DATA) config $(SYSTEMD_CONFIG)
	$(INSTALL_PROGRAM) systemd.wrapper $(SYSTEMD_WRAPPER)
	$(INSTALL_DATA) systemd.service $(SYSTEMD_UNIT)

uninstall-systemd: uninstall-executable
	-rm -f $(SYSTEMD_CONFIG)
	-rm -f $(SYSTEMD_WRAPPER)
	-rm -f $(SYSTEMD_UNIT)

reload-systemd:
	systemctl daemon-reload

