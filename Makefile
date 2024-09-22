# Derleyici
CC = gcc

# Derleme bayrakları
CFLAGS = -Wall -g

# Bağlama bayrakları (ncurses kütüphanesi için)
LDFLAGS = -lncurses

# Hedef dosya
TARGET_DIR = out
TARGET = $(TARGET_DIR)/main

# Kaynak dosyalar
SRCS = main.c hwinfo/hwinfo.c hwinfo/cpu.c hwinfo/gpu.c hwinfo/storage.c swinfo/swinfo.c swinfo/user.c swinfo/de.c swinfo/system.c

# Nesne dosyaları (kaynak dosyalar ile aynı ada sahip .o dosyaları)
OBJS = $(SRCS:.c=.o)

# Varsayılan hedef: bin/main oluşturulacak
all: $(TARGET)

# Hedef: ../bin/main oluşturulacak
$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Hedef: .o dosyalarını derle
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Hedef: Temizlik işlemi (main ve *.o dosyaları silinir, .c dosyaları korunur)
clean:
	rm -f $(OBJS) $(TARGET)

