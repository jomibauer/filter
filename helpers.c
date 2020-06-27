#include "helpers.h"
#include <stdio.h>
#include <math.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // This is straightforward.  It gets an average of all the RGB values for each pixel then changes each RGB value to that average.
            int gray_val = (int) round((float)(image[i][j].rgbtRed + image[i][j].rgbtBlue + image[i][j].rgbtGreen) / 3);
            image[i][j].rgbtRed = gray_val;
            image[i][j].rgbtBlue = gray_val;
            image[i][j].rgbtGreen = gray_val;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    int mid = (int) round((width / 2.0));
    RGBTRIPLE temp = {0, 0, 0};
    
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < mid; j++)
        {
            //This iterates to the halfway point of the picture, mid, which is initalized above.
            //Then we copy the pixel we're swapping to temp, change it to the pixel from the opposite end of the image (line 36, w-j-1)
            // then set the pixel we just swapped to what was stored in temp
            temp = image[i][j];
            image[i][j] = image[i][width - j - 1];
            image[i][width - j - 1] = temp;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE new_image[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // a lot of looping!! but I dont know what else to do for the last two.  We have a sum array up here and a div var which counts
            // how many values get added into the sum array.  We need to keep track because, depending on where the pixel is, we'll skip neighbors
            // (if its an edge). with the second double loop, we loop around the edge of our current pixel and store the RGB vals in the sum
            // array.  
            // At the end of each pixel, we copy the averages as RGB vals into a new_image array, so that we dont affect the image as we're 
            //creating it.  Finally, we copy the new_image into the old one.
            int div = 0.0;
            int sum [3] = {0, 0, 0};
            for (int h = i - 1; h <= i + 1; h++)
            {
                if (h < 0 || h > height - 1)
                {
                    continue;
                }
                for (int w = j - 1; w <= j + 1; w++)
                {
                    if (w < 0 || w > width - 1)
                    {
                        continue;
                    }
                    else
                    {
                        sum[0] += image[h][w].rgbtBlue;
                        sum[1] += image[h][w].rgbtGreen;
                        sum[2] += image[h][w].rgbtRed;
                        div++;
                    }
                }
            }
            
            new_image[i][j].rgbtBlue = (int) round((float) sum[0] / div);
            new_image[i][j].rgbtGreen = (int) round((float) sum[1] / div);
            new_image[i][j].rgbtRed = (int) round((float) sum[2] / div);
        }
        
        
    }
    for (int x = 0; x < height; x++)
    {
        for (int y = 0; y < width; y++)
        {
            image[x][y] = new_image[x][y];
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE new_image[height][width];
    int Kx[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
    int Ky[3][3] = { {-1, -2, -1}, {0, 0, 0},  {1, 2, 1} };
    //These are arrays for our kernels.  
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int final_vals[3] = {0, 0, 0};
            float g_xvals[3] = {0, 0, 0};
            float g_yvals[3] = {0, 0, 0};
            // like before, we need to visit neighbors, only the operations are changing.  We're getting values for each RGB from each neighbor
            // and multiplying by a constant from the kernel array.  Then we add those up into the g_vals arrays, and then at the end of each
            // loop per pixel, we do the little sqrt, pow thing that combines Gx and GY into three RGB vals for our target bmp.  
            // as before, we have to store these values in a new_image, only copying them into the new bmp once we're all done.
            for (int row = i - 1, x = 0; row <= i + 1; row++, x++)
            {
                for (int col = j - 1, y = 0; col <= j + 1; col++, y++)
                {
                    if ((row < 0 || row >= height) || (col < 0 || col >= width))
                    {
                        continue;
                    }
                    else
                    {
                        g_xvals[0] += Kx[x][y] * image[row][col].rgbtBlue;
                        g_xvals[1] += Kx[x][y] * image[row][col].rgbtGreen;
                        g_xvals[2] += Kx[x][y] * image[row][col].rgbtRed;
                        
                        g_yvals[0] += Ky[x][y] * image[row][col].rgbtBlue;
                        g_yvals[1] += Ky[x][y] * image[row][col].rgbtGreen;
                        g_yvals[2] += Ky[x][y] * image[row][col].rgbtRed;
                    }
                }
            }
            for (int n = 0; n < 3; n++)
            {
                final_vals[n] = (int) round(sqrt(pow(g_xvals[n], 2) + pow(g_yvals[n], 2)));
                if (final_vals[n] > 255)
                {
                    final_vals[n] = 255;
                }
                
            }
            new_image[i][j].rgbtBlue = final_vals[0];
            new_image[i][j].rgbtGreen = final_vals[1];
            new_image[i][j].rgbtRed = final_vals[2];    
        }
    }
    for (int new_i = 0; new_i < height; new_i++)
    {
        for (int new_j = 0; new_j < width; new_j++)
        {
            image[new_i][new_j] = new_image[new_i][new_j];
        }
    }
    return;
}
