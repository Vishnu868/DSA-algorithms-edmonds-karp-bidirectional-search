CC = gcc

SOURCES = bdmain.c bdgraph.c

TARGET = bdprogram

run: $(TARGET)
	echo "links.txt\nhttp://example.com\nhttp://example.com/blog/post5\nno" | ./$(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean
