BUILD_DIR = build
INCLUDE_DIR = include
SOURCE_DIR = src
TARGET_DLL = py_inj.dll

SOURCES = $(notdir $(wildcard $(SOURCE_DIR)/*.cpp))
OBJECTS = $(subst .cpp,.obj,$(SOURCES))
OBJECT_FILES = $(addprefix $(BUILD_DIR)/,$(OBJECTS))
INCLUDE_FILES = $(wildcard $(INCLUDE_DIR)/*.h)
CXX = cl
CXXFLAGS = /c /I$(INCLUDE_DIR) /EHsc /std:c++latest /Wall /wd4668 /wd5039 /nologo
LD = link
LDFLAGS = /DLL /nologo

.PHONY: all
all: $(BUILD_DIR)/$(TARGET_DLL)

.PHONY: clean
clean:
	@if exist $(BUILD_DIR)/ rmdir $(BUILD_DIR) /s /q

$(BUILD_DIR)/$(TARGET_DLL): $(OBJECT_FILES)
	$(LD) $^ /OUT:$@ $(LDFLAGS)

$(BUILD_DIR)/%.obj: $(SOURCE_DIR)/%.cpp $(INCLUDE_FILES) | $(BUILD_DIR)
	$(CXX) $< /Fo: $@ $(CXXFLAGS)

$(BUILD_DIR):
	@mkdir $@
