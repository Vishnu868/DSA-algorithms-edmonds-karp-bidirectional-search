CC = gcc

SOURCES = edmain.c edgraph.c

TARGET = program

run: $(TARGET)
	echo "links.txt\nhttp://example.com\nhttp://example.com/blog/post5" | ./$(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean
