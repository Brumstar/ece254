# README

## Background

Bitmap contains 1 or 0 for each four bytes of memory available.
1 means allocated, 0 means unallocated
So for 1024 byte alloc, need 1024 / 4 = 256 bits in bitmap to show state of all blocks
256 bits to bytes is 256 / 8 = 32 bytes necessary for the bitmap.
print_bitmap() function will print the state of every bit in the bitmap.
Each allocated block needs a 4 byte padding, which is 0 in the bitmap, to be able to differentiate the ends of blocks of allocated memory.
So the 4 bytes of padding need to be subtracted from the free space available, and returned when dealloc is called.
