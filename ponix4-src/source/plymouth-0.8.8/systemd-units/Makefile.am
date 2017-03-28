systemd_unit_templates =                                                     \
        plymouth-switch-root.service.in                                      \
        plymouth-start.service.in                                            \
        plymouth-read-write.service.in                                       \
        plymouth-quit.service.in                                             \
        plymouth-quit-wait.service.in                                        \
        plymouth-reboot.service.in                                           \
        plymouth-kexec.service.in                                            \
        plymouth-poweroff.service.in                                         \
        plymouth-halt.service.in                                             \
        systemd-ask-password-plymouth.path.in                                \
        systemd-ask-password-plymouth.service.in

if ENABLE_SYSTEMD_INTEGRATION
systemdunitdir=$(SYSTEMD_UNIT_DIR)
systemdunit_DATA = $(systemd_unit_templates:.in=)

install-data-hook:
	$(MKDIR_P) -m 0755                                                   \
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/initrd-switch-root.target.wants\
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/sysinit.target.wants           \
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/multi-user.target.wants        \
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/reboot.target.wants            \
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/kexec.target.wants             \
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/poweroff.target.wants          \
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/halt.target.wants
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/initrd-switch-root.target.wants && \
		rm -f plymouth-switch-root.service &&                        \
		$(LN_S) ../plymouth-switch-root.service)
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/sysinit.target.wants &&            \
		rm -f plymouth-start.service plymouth-read-write.service &&  \
		$(LN_S) ../plymouth-start.service &&                         \
		$(LN_S) ../plymouth-read-write.service)
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/multi-user.target.wants &&         \
		rm -f plymouth-quit.service plymouth-quit-wait.service &&    \
		$(LN_S) ../plymouth-quit.service &&                          \
		$(LN_S) ../plymouth-quit-wait.service)
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/reboot.target.wants &&             \
		rm -f plymouth-reboot.service &&                             \
		$(LN_S) ../plymouth-reboot.service)
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/kexec.target.wants &&              \
		rm -f plymouth-kexec.service &&                              \
		$(LN_S) ../plymouth-kexec.service)
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/poweroff.target.wants &&           \
		rm -f plymouth-poweroff.service &&                           \
		$(LN_S) ../plymouth-poweroff.service)
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/halt.target.wants &&               \
		rm -f plymouth-halt.service &&                               \
		$(LN_S) ../plymouth-halt.service)

uninstall-hook:
	rm -f \
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/initrd-switch-root.target.wants && \
		rm -f plymouth-switch-root.service)                          \
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/sysinit.target.wants &&            \
		rm -f plymouth-start.service plymouth-read-write.service)    \
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/multi-user.target.wants &&         \
		rm -f plymouth-quit.service plymouth-quit-wait.service)      \
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/reboot.target.wants &&             \
		rm -f plymouth-reboot.service)                               \
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/kexec.target.wants &&              \
		rm -f plymouth-kexec.service)                                \
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/poweroff.target.wants &&           \
		rm -f plymouth-poweroff.service)                             \
	(cd $(DESTDIR)$(SYSTEMD_UNIT_DIR)/halt.target.wants &&               \
		rm -f plymouth-halt.service)                                 \
	rmdir --ignore-fail-on-non-empty                                     \
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/sysinit.target.wants           \
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/multi-user.target.wants        \
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/reboot.target.wants            \
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/kexec.target.wants             \
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/poweroff.target.wants          \
		$(DESTDIR)$(SYSTEMD_UNIT_DIR)/halt.target.wants

endif

EXTRA_DIST = $(systemd_unit_templates) $(systemdunit_DATA)
DISTCLEANFILES=$(systemdunit_DATA)
