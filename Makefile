BUILD_DIR = build
TARGET_DLL = $(BUILD_DIR)/balmora_dll.dll
CXX = cl
CXXFLAGS = /LD /EHsc /std:c++20 /link /out:$(TARGET_DLL)
BALMORA_DIR = c:\Users\Michal\Desktop\Balmora_21022023\

.PHONY: all
all: $(TARGET_DLL)

.PHONY: copy
copy: $(TARGET_DLL)
	copy $(TARGET_DLL) $(BALMORA_DIR)

.PHONY: clean
clean:
	@if exist $(BUILD_DIR)/ rmdir $(BUILD_DIR) /s /q

$(TARGET_DLL): balmora_dll.cpp | $(BUILD_DIR)
	$(CXX) $^ $(CXXFLAGS)
	@del balmora_dll.obj

$(BUILD_DIR):
	@mkdir $@
