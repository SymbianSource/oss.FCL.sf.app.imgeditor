ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\RESOURCE\APPS
HEADERDIR=$(EPOCROOT)epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\Icons_doodle.mif
HEADERFILENAME=$(HEADERDIR)\Icons_doodle.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIN : do_nothing

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
	/c32,8  ..\data\palette_colored.svg\
	/c32,8  ..\data\undo.svg\
	/c32,8  ..\data\redo.svg\
	/c32,8  ..\data\exit.svg\
	/c32,8  ..\data\info.svg\
	/c32,8  ..\data\pen.svg\
	/c32,8  ..\data\save1.svg\
	/c32,8  ..\data\save2.svg
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME) && \
	@echo $(ICONTARGETFILENAME)
FINAL : do_nothing