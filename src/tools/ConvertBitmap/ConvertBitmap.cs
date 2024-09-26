using System;
using System.Drawing;
using System.IO;

class ConvertBitmap
{
    static void Main(string[] args)
    {
        if(args.Length < 2)
        {
            return;
        }
        string filePath = args[0];
        Console.WriteLine(filePath);

        int bitmapW, bitmapH;
        byte[] rawBytes;
        int byteWidth;
        int byteHeight;

        // Load the image
        using (Bitmap bitmap = new Bitmap(filePath))
        {
            bitmapW = bitmap.Width;
            bitmapH = bitmap.Height;
            using (MemoryStream bitStream = new MemoryStream())
            {
                int bitCount = 0;
                byte currentByte = 0;

                // Iterate through each pixel
                for (int y = 0; y < bitmap.Height; y++)
                {
                    for (int x = 0; x < bitmap.Width; x++)
                    {
                        // Get pixel color
                        Color pixelColor = bitmap.GetPixel(x, y);
                        
                        // Determine if pixel is black or white
                        // For simplicity, assume if the color is closer to white, it's white, otherwise it's black
                        bool isWhite = pixelColor.R > 127 && pixelColor.G > 127 && pixelColor.B > 127;
                        
                        // Shift current byte and add the bit
                        currentByte <<= 1;
                        if (isWhite)
                        {
                            currentByte |= 1; // Set last bit to 1 for white
                        }

                        bitCount++;
                        
                        // Once we have 8 bits, write the byte to the stream
                        if (bitCount == 8)
                        {
                            bitStream.WriteByte(currentByte);
                            bitCount = 0;
                            currentByte = 0;
                        }
                    }
                }

                // If there are leftover bits, pad the last byte with 0s and write it
                if (bitCount > 0)
                {
                    currentByte <<= (8 - bitCount); // Shift to fill remaining bits with 0
                    bitStream.WriteByte(currentByte);
                }

                  rawBytes = bitStream.ToArray();
            }
            byteWidth = bitmapW / 8;
            byteHeight = bitmapH / 8;
            int startIndex = 0;


            using (MemoryStream bitStream = new MemoryStream())
            {
                for(int y = 0; y < byteHeight; y++)
                {
                    for(int x = 0; x < byteWidth; x++)
                    {

                        for(int z = 0; z < 8; z++)
                        {
                            var index = startIndex + x + z * byteWidth;
                            bitStream.WriteByte(rawBytes[index]);
                            Console.WriteLine(index);
                        }    
                    }
                    startIndex += byteWidth * 8;
                }

                // Write the stream to a file, or process as needed
                File.WriteAllBytes(args[1], bitStream.ToArray());
            }

            Console.WriteLine(args[1]);
            Console.WriteLine("Image has been processed and stored in a bit stream.");
        }
    }
}
