## Copyright (c) 2013 Comcast. All rights reserved.

SUBDIRS = src include
DIST_SUBDIRS = cfg src include

distcleancheck_listfiles = *-libtool
sysconf_DATA= debug.ini log4crc

### IPK_GENERATION_PACKAGE###
IPK_GEN_PATH = $(abs_top_builddir)/ipk
IPK_GEN_STAGING_DIR=$(abs_top_builddir)/staging_dir
IPK_GEN_INPUT  = debian-binary control.tar.gz data.tar.gz
IPK_GEN_OUTPUT = librdklogger.ipk

package_ipk:
	@echo "Generating IPK Package"
	$(MAKE) prefix=${abs_top_builddir}/staging_dir install
	$(mkdir_p) $(IPK_GEN_STAGING_DIR)/usr/lib
	$(mkdir_p) $(IPK_GEN_STAGING_DIR)/etc
	cp -af $(abs_top_builddir)/log4crc $(IPK_GEN_STAGING_DIR)/etc/
	cp -af $(abs_top_builddir)/debug.ini $(IPK_GEN_STAGING_DIR)/etc/
	cp -af $(IPK_GEN_STAGING_DIR)/lib/*.so* $(IPK_GEN_STAGING_DIR)/usr/lib/
	tar -czvf $(IPK_GEN_PATH)/data.tar.gz -C $(IPK_GEN_STAGING_DIR) usr etc
	tar -czvf $(IPK_GEN_PATH)/control.tar.gz -C $(IPK_GEN_PATH) control
	cd $(IPK_GEN_PATH) && ar cr $(IPK_GEN_OUTPUT) $(IPK_GEN_INPUT) && cd -
	rm -rf $(IPK_GEN_PATH)/data.tar.gz
	rm -rf $(IPK_GEN_PATH)/control.tar.gz
	rm -rf $(IPK_GEN_STAGING_DIR)
