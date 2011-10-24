TESTDIR = /mnt/ToWin/adms_scripts_2
TESTFILE = ngspiceMODULEdefs.h.xml

ALL_XML = `ls /mnt/ToWin/adms_scripts/ngspice*.xml`

# not to change:
# makehumancripts:
XML_FILES = `cd /mnt/ToWin/adms_scripts && ls ngspice*.xml`

# makeincludescrit:
MIS = ../Build/include_script.xml
MIS_CX = ../adms_scripts_ln/include_script_cx.xml
MIS_F = `cd ../adms_include && ls *.c` \
	`cd ../adms_include && ls *.h`

all:
	g++ convert.cc -o convert
outside:
	cp /mnt/ToWin/convert.cpp ./convert.cc
	g++ convert.cc -o convert
testfile:
	@echo "test file: $(TESTFILE)"
	./convert $(TESTDIR)/$(TESTFILE) -o $(TESTDIR)/out.xml -removemotxml
	./convert $(TESTDIR)/out.xml -o $(TESTDIR)/out2.xml -addxml
	diff $(TESTDIR)/$(TESTFILE) $(TESTDIR)/out2.xml
testall:
	for FILE in $(ALL_XML); do \
		echo; \
		echo "***** file: $$FILE:"; \
		echo; \
		./convert $$FILE -o $(TESTDIR)/out.xml -removemotxml; \
		./convert $(TESTDIR)/out.xml -o $(TESTDIR)/out2.xml -addxml; \
		diff $$FILE $(TESTDIR)/out2.xml; \
	done

makehumanscripts:
	-mkdir /mnt/ToWin/human_scripts
	for FILE in $(XML_FILES); do \
		echo "convert file: $$FILE"; \
		./convert /mnt/ToWin/adms_scripts/$$FILE -o /mnt/ToWin/human_scripts/$$FILE -removemotxml; \
	done

makeincludescript:
	@echo -e -n "<admst>\n" > $(MIS)
	for FILE in $(MIS_F); do \
		echo "add file: $$FILE"; \
		echo -e -n "<admst:template match=\"PrintFile$$FILE\">\n" >> $(MIS); \
		less ../adms_include/$$FILE >> $(MIS); \
		echo -e -n "<admst:text format=\"\\n\"/>" >> $(MIS); \
		echo -e -n "</admst:template>\n" >> $(MIS); \
	done
	for FILE in $(MIS_F); do \
		echo -e -n "\t<admst:open file=\"$$FILE\">\n" >> $(MIS); \
		echo -e -n "\t\t<admst:apply-templates select=\".\" match=\"PrintFile$$FILE\"/>\n" >> $(MIS); \
		echo -e -n "\t</admst:open>\n" >> $(MIS); \
	done
	@echo -e -n "</admst>\n" >> $(MIS)

	./convert $(MIS) -o $(MIS_CX) -fixc
