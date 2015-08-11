/*  
    The file TerrainMap.cs is part of AGMGSKSKv6 
    Academic Graphics Starter Kit version 6 for MonoGames 3.2 or XNA 4 refresh
  
    Jared Fowler - Febuary 3, 2015 - Modifications made to this file include de-tabifying everything, and replacing all
    "tabs" with 4 spaces. The two functions, createHeightTexture ; createColorTexture, were also modified to relfect
    my chosen enviroments.  Other functions have been modified for better readability. (Commentaries, spaces, etc.)
    Structure added to allow greater flexibility for terrain creation. Different options allowed for circular, diamond
    formed height increases.  
    Jared Fowler - February 10, 2015 - Modified to allow creation of space, moon-looking enviroment. Algorith for craters added.
   
    Mike Barnes
    1/21/2015

    AGMGSKv6 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

# region Using Statements
using System;
using System.IO;  // needed for TerrainMap's use of Stream class in saveTerrainAsText()
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
# if MONOGAMES  
	using Microsoft.Xna.Framework.Storage;
# endif
# endregion

namespace TerrainMap {

/// <summary>
/// XN4 project that can create terrain data textures.
/// MonoGame project use, see note at end of summary.
/// 
/// Generate and save two 2D textures:  heightTexture.png and colorTexture.png.
/// File heightTexture.png stores a terrain's height values 0..255.
/// File colorTexture.png stores the terrain's vertex color values.
/// The files are saved in the execution directory.
/// 
/// Pressing 't' will toggle the display between the height and color
/// texture maps.  As distributed, the heightTexture will look all black
/// because the values range from 0 to 3.
/// 
/// The heightTexture will be mostly black since in the SK565v3 release there
/// are two height areas:  grass plain and pyramid.  The pyramid (upper left corner)'
/// will show grayscale values. 
/// Grass height values range from 0..3 -- which is black in greyscale.
/// 
/// Note:  using grayscale in a texture to represent height constrains the 
/// range of heights from 0 to 255.  Often you need to scale the values into this range
/// before saving the texture.  In your world's terrain you can then scale these 
/// values to the range you want.  This program does not scale since no values
/// become greater than 255.
/// 
/// Normally one thinks of a 2D texture as having [u, v] coordinates. 
/// In createHeightTexture() the height and in createColorTexture the color 
/// values are created.
/// The heightMap and colorMap used are [u, v] -- 2D.  They are converted to a 
/// 1D textureMap1D[u*v] when the colorTexture's values are set.
/// This is necessary because the method
///       newTexture.SetData<Color>(textureMap1D);
/// requires a 1D array, not a 2D array.
/// 
/// Program design was influenced by Riemer Grootjans example 3.7
/// Create a texture and save to file.
/// In XNA 2.0 Grame Programming Recipies:  A Problem-Solution Approach,
/// pp 176-178, Apress, 2008.
/// 
/// MonoGames can write textures using System.Drawing.Color and System.Drawing.Bitmap
/// classes.  You need to add a reference for System.Drawing in Visual Studio or MonoDevelop
///  
/// Mike Barnes
/// 1/222015
/// </summary>

    //####################################################################################################

    public class TerrainMap : Game
    {

        //Stuctures----------------------------------------------------------------
        struct HeightStruct   //Used within the TextureHeight Method
        {
            public UInt16 centerX;
            public UInt16 centerZ;
            public UInt16 fillRadius;
            public UInt16 steps;
            public UInt16 stepSize;

            public HeightStruct(UInt16 centerX, UInt16 centerZ, UInt16 fillRadius, UInt16 steps, UInt16 stepSize)
            {
                this.centerX = centerX;
                this.centerZ = centerZ;
                this.fillRadius = fillRadius;
                this.steps = steps;
                this.stepSize = stepSize;
            }
        }

        struct CraterStruct  //Variables used for creations of craters in the map
        {
            public UInt16 centerX;
            public UInt16 centerZ;
            public UInt16 fillRadiusHi;  //Top radius of hole
            public UInt16 fillRadiusLo;  //Bottom radius of hole

            public CraterStruct(UInt16 centerX, UInt16 centerZ, UInt16 fillRadiusHi, UInt16 fillRadiusLo)
            {
                this.centerX = centerX;
                this.centerZ = centerZ;
                this.fillRadiusHi = fillRadiusHi;
                this.fillRadiusLo = fillRadiusLo;
            }
        }

        //Constants----------------------------------------------------------------
        //Texture should be powers of 2 for mipmapping
        private const UInt16 TEXTURE_WIDTH  = 512;
        private const UInt16 TEXTURE_HEIGHT = 512;

        //Variables----------------------------------------------------------------
        private GraphicsDeviceManager graphics;
        private GraphicsDevice device;
        private SpriteBatch spriteBatch;

        private Texture2D heightTexture;    // Our desired output texture (rendered on screen via program)
        private Texture2D colorTexture;     // Our desired output texture (rendered on screen via program)
        private Color[,] colorMap;
        private Color[,] heightMap;
        private Color[] textureMap1D;     // Hold the generated values for a texture.
        private Random random;
        private bool showHeight = false;

        private KeyboardState oldState;

        //####################################################################################################
        /// <summary>
        /// Constructor
        /// </summary>
        public TerrainMap()
        {
            graphics = new GraphicsDeviceManager(this);
            //Set the windows title
            Window.Title = "Terrain Maps " + TEXTURE_WIDTH + " by " + TEXTURE_HEIGHT + " to change map 't'";
            //Set root directory for content
            Content.RootDirectory = "Content";
            //Instantiate and seed the random number generator
            random = new Random();
        }

        //####################################################################################################
        /// <summary>
        /// Set the window size based on the texture dimensions.
        /// </summary>

        protected override void Initialize()
        {
            // Game object exists, set its window size 
            graphics.PreferredBackBufferWidth = TEXTURE_WIDTH;
            graphics.PreferredBackBufferHeight = TEXTURE_HEIGHT;
            graphics.ApplyChanges();
            base.Initialize();
        }

        //####################################################################################################
        /// <summary>
        /// Create and save two textures:  
        ///   heightTexture.png 
        ///   colorTexture.png
        /// </summary>

        protected override void LoadContent()
        {
            // Create a new SpriteBatch, which can be used to draw textures.
            spriteBatch = new SpriteBatch(GraphicsDevice);
            device = graphics.GraphicsDevice;
            heightTexture = createHeightTexture();
            colorTexture = createColorTexture();
            saveTexture(heightMap, "heightTexture.png");
            saveTexture(colorMap, "colorTexture.png");
        }

        //####################################################################################################
        /// <summary>
        /// Uses .Net System.Drawing.Bitmap and System.Drawing.Color to create
        /// png image files.
        /// </summary>
        /// <param name="map"> Color [width, height] values for texture </param>
        /// <param name="filename"> texture's nanme</param>
        private void saveTexture(Color[,] map, string filename)
        {
            System.Drawing.Color color;
            System.Drawing.Bitmap image = new System.Drawing.Bitmap(TEXTURE_WIDTH, TEXTURE_HEIGHT);

            for (int x = 0; x < TEXTURE_WIDTH; x++)
                for (int z = 0; z < TEXTURE_HEIGHT; z++)
                {
                    color = System.Drawing.Color.FromArgb(Convert.ToInt32(map[x, z].R),
                                                          Convert.ToInt32(map[x, z].G),
                                                          Convert.ToInt32(map[x, z].B)
                                                          );
                    image.SetPixel(x, z, color);
                }

            image.Save(filename, System.Drawing.Imaging.ImageFormat.Png);

        }

        //####################################################################################################
        private Texture2D createHeightTexture()
        {
            /* J. Fowler - 
             * The terrain will be made to look like a moon-like, outer space planet. Order of application will be as follows:
             * 1. General Height given to the entire map
             * 2. Brownian Algorithm to generate mountains
             * 3. Crater maker algorithm (Flat area will be in crater)
             * 4. Final Randomizer
             * 5. Make Maze area on a small platuo, very flat
             * 
             * The map will be very interesting when it is finished. We just need to make sure that we create
             * a flat place (step 5) for the maze which we will put in via AGMSK
             * 
             */

            //Local Variables
            float floatHolder;
            Int32 x, z, m, n, p;
            Vector3 colorVec3;
            UInt32 i, j, k, h;
            Int32[,] integerHeight;
            Int32 height = 0;

            //Initialize objects
            heightMap     = new Color[TEXTURE_WIDTH, TEXTURE_HEIGHT];
            textureMap1D  = new Color[TEXTURE_WIDTH * TEXTURE_HEIGHT];
            integerHeight = new Int32[TEXTURE_WIDTH, TEXTURE_HEIGHT];


            //The following controls the output of the terrain height (if modified should be done with logical care.)
            UInt16 valley_range_hi = 100;    //Max height of valley floor 
            UInt16 valley_range_lo = 90;    //Min height of valley floor
            UInt16 passes = 2;    //Number of times to apply Brownian Algorithm

            HeightStruct[] heightStruct = { new HeightStruct(20, 20, 150, 50, 20),  //Note: x,z are shown backwards in program, but saved image file is correct
                                            new HeightStruct(250, 20, 150, 50, 20),
                                            new HeightStruct(20, 250, 150, 50, 20),
                                            new HeightStruct(20, 450, 150, 50, 20),
                                            //new HeightStruct(450, 20, 150, 50, 20),
                                            new HeightStruct(450, 450, 200, 25, 20),
                                          };

            CraterStruct[] craterStruct = { 
                                            new CraterStruct(400, 400, 200, 50),
                                            new CraterStruct(100, 100, 100, 10),
                                          };


            //First we'll want to create a base height for all the map. (Valley Floor)
            for (x = 0; x < TEXTURE_WIDTH; x++)
            {
                for (z = 0; z < TEXTURE_HEIGHT; z++)
                {
                    //Generate the random height value
                    integerHeight[x, z] = random.Next(valley_range_hi - valley_range_lo) + valley_range_lo;
                }
            }

            /* The Brownian Algorithm, as called by Professor Barnes, will be used to generate the higher land masses which exist in the terrain. 
             * The idea of the algorithm is to begin a loop which does the following on each iteration: First, given a 'center', update (raise or lower)
             * the land height pertaining to the radius surrounding the center. Then we simply update the center location by stepping some distance
             * 'stepSize' in both the x and z directions. If we happen to step out of bounds, we simply restart at the original center point. This continues
             * for a user specified number of times. */

            for (i = 0; i < passes; i++)
            {
                for (j = 0; j < heightStruct.Length; j++)
                {
                    //Assign x,z to the center we are working with this iteration
                    x = heightStruct[j].centerX;
                    z = heightStruct[j].centerZ;

                    for (k = 0; k < heightStruct[j].steps; k++)
                    {
                        /* The problem we encounter here is that part of the radius we want to raise may not exist within bounds of 
                         * the texture. We will work around this via various techniques, avoiding the idea of just going through 
                         * the entire structure in a brute force approach. For a circle, we will enclose it within a square, and 
                         * then brute force the square. For a diamond, we will be a little bit more sneaky. */

                        if (random.Next(10) > 2)  //Do circle technique
                        {
                            for (m = x - heightStruct[j].fillRadius; m < (x + heightStruct[j].fillRadius); m++)
                            {
                                //Are we out of terrain bounds?
                                if (m < 0)
                                    continue;
                                if (m >= TEXTURE_WIDTH)
                                    break;

                                for (n = z - heightStruct[j].fillRadius; n < (z + heightStruct[j].fillRadius); n++)
                                {
                                    //If out of bounds, continue with next iteration
                                    if ((n > (TEXTURE_HEIGHT - 1)) || (n < 0))
                                        continue;

                                    //Are we within radius distance of the center point?
                                    if (heightStruct[j].fillRadius >= (Math.Sqrt((Math.Pow((m - x), 2) + Math.Pow((n - z), 2)))))
                                        if(integerHeight[m,n] < 255)
                                            integerHeight[m, n] += 1;
                                }
                            }


                        }
                        else  //Do diamond technique
                        {
                            //Upper portion of diamond
                            p = 0;
                            for (m = z; m >= (z - heightStruct[j].fillRadius); m--)
                            {
                                //Are we out of terrain bounds?
                                if (m < 0)
                                    break;

                                for (n = (x - heightStruct[j].fillRadius + p); n <= (x + heightStruct[j].fillRadius - p); n++)
                                {
                                    //Are we out of terrain bounds?
                                    if ((n > (TEXTURE_WIDTH - 1)) || (n < 0))
                                        continue;

                                    //Else, we will increase the height here
                                    if (integerHeight[m, n] < 255)
                                        integerHeight[n, m] += 1;
                                }

                                //Update p, which keeps track of iterations
                                p++;
                            }

                            //Lower portion of diamond, excluding center diameter
                            p = 0;
                            for (m = (z + 1); m <= (z + heightStruct[j].fillRadius); m++)
                            {
                                //Are we out of terrain bounds?
                                if (m > (TEXTURE_HEIGHT - 1))
                                    break;

                                for (n = (x - heightStruct[j].fillRadius + p); n <= (x + heightStruct[j].fillRadius - p); n++)
                                {
                                    //Are we out of terrain bounds?
                                    if ((n > (TEXTURE_WIDTH - 1)) || (n < 0))
                                        continue;

                                    //Else, we will increase the height here
                                    if (integerHeight[m, n] < 255)
                                        integerHeight[n, m] += 1;
                                }

                                //Update p, which keeps track of iterations
                                p++;
                            }
                        }

                        //Take a random step in both the x and z directions
                        x += random.Next(-(heightStruct[j].stepSize), heightStruct[j].stepSize);
                        z += random.Next(-(heightStruct[j].stepSize), heightStruct[j].stepSize);


                        //Are we out of texture bounderies? If so, replace our position on center
                        if ((x >= TEXTURE_WIDTH || x < 0) || (z >= TEXTURE_HEIGHT || z < 0))
                        {
                            x = heightStruct[j].centerX;
                            z = heightStruct[j].centerZ;
                        }

                    }
                }
            }

            //Add craters to the map
            for (i = 0; i < craterStruct.Length; i++)
            {
                //Get the total peremeter height
                height = 0;
                j = 0;
                for (m = craterStruct[i].centerX - craterStruct[i].fillRadiusHi; m < craterStruct[i].centerX + craterStruct[i].fillRadiusHi; m++)
                {
                    //Are we out of terrain bounds?
                    if (m < 0)
                        continue;
                    if (m >= TEXTURE_WIDTH)
                        break;

                    for (n = craterStruct[i].centerZ - craterStruct[i].fillRadiusHi; n < craterStruct[i].centerZ + craterStruct[i].fillRadiusHi; n++)
                    {
                        //If out of bounds, continue with next iteration
                        if ((n > (TEXTURE_HEIGHT - 1)) || (n < 0))
                            continue;

                        //Increase our height, and addition count
                        height = (height + integerHeight[m, n]);
                        j++;

                    }
                }

                //Get the avg. peremeter height
                height /= (Int32)j;

                    for (j = craterStruct[i].fillRadiusHi; j > craterStruct[i].fillRadiusLo; j -= 1)
                    {
                        for (m = craterStruct[i].centerX - (Int32)j; m < craterStruct[i].centerX + j; m++)
                        {
                            //Are we out of terrain bounds?
                            if (m < 0)
                                continue;
                            if (m >= TEXTURE_WIDTH)
                                break;

                            for (n = craterStruct[i].centerZ - (Int32)j; n < craterStruct[i].centerZ + j; n++)
                            {
                                //If out of bounds, continue with next iteration
                                if ((n > (TEXTURE_HEIGHT - 1)) || (n < 0))
                                    continue;

                                //Are we within radius distance of the center point?
                                if (j >= (Math.Sqrt((Math.Pow((m - craterStruct[i].centerX), 2) + Math.Pow((n - craterStruct[i].centerZ), 2)))))
                                    if (integerHeight[m, n] > 3)
                                        integerHeight[m, n] = height;

                            }
                        }

                        //Decrease the next level's height
                        height -= 1;
                    }
            }

            //Randomizer
            for (x = 0; x < TEXTURE_WIDTH; x++)
            {
                for (z = 0; z < TEXTURE_HEIGHT; z++)
                {
                    if(random.Next(2) == 1)
                        integerHeight[x, z] += random.Next(5);
                    else
                        integerHeight[x, z] -= random.Next(5);

                    //Make sure we don't step under/over bounds
                    if (integerHeight[x, z] <= 0)
                        integerHeight[x, z] = 5;
                    if (integerHeight[x, z] > 255)
                        integerHeight[x, z] = 255;
                }
            }

            //Flat platuo for wall object in ASMGSK
            //Get the total perimeter height
            height = 0;
            j = 0;
            for (m = 435; m < 465; m++)
            {
                //Are we out of terrain bounds?
                if (m < 0)
                    continue;
                if (m >= TEXTURE_WIDTH)
                    break;

                for (n = 435; n < 465; n++)
                {
                    //If out of bounds, continue with next iteration
                    if ((n > (TEXTURE_HEIGHT - 1)) || (n < 0))
                        continue;

                    //Increase our height, and addition count
                    height = (height + integerHeight[m, n]);
                    j++;

                }
            }

            //Get the avg. peremeter height
            height /= (Int32)j;

            for (m = 430; m < 470; m++)
            {
                //Are we out of terrain bounds?
                if (m < 0)
                    continue;
                if (m >= TEXTURE_WIDTH)
                    break;

                for (n = 430; n < 470; n++)
                {
                    //If out of bounds, continue with next iteration
                    if ((n > (TEXTURE_HEIGHT - 1)) || (n < 0))
                        continue;

                    if (20 >= (Math.Sqrt((Math.Pow((m - 450), 2) + Math.Pow((n - 450), 2)))))
                        integerHeight[m, n] = height;
                    

                }
            }





            //We need to convert the integer map into heightMap values
            for (m = 0; m < TEXTURE_HEIGHT; m++)
            {
                for (n = 0; n < TEXTURE_WIDTH; n++)
                {
                    floatHolder = ((float)integerHeight[n, m] / 255.0f);
                    colorVec3 = new Vector3(floatHolder, floatHolder, floatHolder);
                    heightMap[n, m] = new Color(colorVec3);

                }
            }

            //Convert heightMap[,] to textureMap1D[]
            i = 0;
            for (x = 0; x < TEXTURE_WIDTH; x++)
            {
                for (z = 0; z < TEXTURE_HEIGHT; z++)
                {
                    textureMap1D[i] = heightMap[x, z];
                    i++;
                }
            }

            //Create the texture to return.       
            Texture2D newTexture = new Texture2D(device, TEXTURE_WIDTH, TEXTURE_HEIGHT);
            newTexture.SetData<Color>(textureMap1D);
            return newTexture;
        }

        //####################################################################################################
        /// <summary>
        /// Return random int -range ... range
        /// </summary>
        /// <param name="range"></param>
        /// <returns></returns>
        private int fractalRand(int range)
        {
            if (random.Next(2) == 0)  // flip a coin
                return (random.Next(range));
            else
                return (-1 * (random.Next(range)));
        }

        //####################################################################################################
        /// <summary>
        /// Convert a height value in the range of 0 ... 255 to
        /// a Vector4 value that will be later converted to a Color.  
        /// Vector4 is used instead of color to add some random noise to the value
        /// </summary>
        /// <param name="h"></param>
        /// <returns></returns>
        private Vector4 heightToVector4(int h)
        {
            //Local Variables
            Int32 r, g, b;

            //Most of the following table has been carried over from what Professor Barnes provided.


            if(h < 5) //Dark Gray or black
            {
                r = 50 + random.Next(10);
                g = 50 + random.Next(10);
                b = 50 + random.Next(10);
            }
            else if (h < 50) //Gray
            {
                r = 90 + random.Next(10);
                g = 90 + random.Next(10);
                b = 90 + random.Next(10);
            }
            else if (h < 75) //Dark Gray
            {
                r = 169 + random.Next(10);
                g = 169 + random.Next(10);
                b = 169 + random.Next(10);
            }
            else if (h < 100) //gray 74
            { 
                r = 189 + random.Next(10);
                g = 189 + random.Next(10);
                b = 189 + random.Next(10);
            }
            else if (h < 125) //Gray 69
            {  
                r = 69 + random.Next(10);
                g = 69 + random.Next(10);
                b = 69 + random.Next(10);
            }
            else if (h < 150) //Darkish
            {  
                r = 50 - random.Next(10);
                g = 50 - random.Next(10);
                b = 50 - random.Next(10);
            }
            else if (h < 175) //Gray 50
            { 
                r = 50 - random.Next(20);
                g = 50 - random.Next(20);
                b = 50 - random.Next(20);
            }
            else if (h < 225)    // Light Gray
                r = g = b = 150 + random.Next(30);

            else    //Gray
                r = g = b = 150 + random.Next(30); ;

            // add noise with fractalRand
            if (h <= 175)
            {
                // randomize values and clamp values to 0..255
                r = Math.Abs((r + fractalRand(20)) % 255);
                g = Math.Abs((g + fractalRand(20)) % 255);
                b = Math.Abs((b + fractalRand(20)) % 255);
            }
            else if (h > 175 && h < 225)
                r = g = b = Math.Abs((r + fractalRand(20)) % 255);

            return new Vector4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);  // must be floats
        }

        //####################################################################################################
        /// <summary>
        /// Create a color texture that will be used to "color" the terrain.
        /// Some comments about color that might explain some of the code in createColorTexture().
        /// Colors can be converted to vector4s.   vector4Value =  colorValue / 255.0
        /// color's (RGBA), color.ToVector4()
        /// Color.DarkGreen (R:0 G:100 B:0 A:255)    vector4 (X:0 Y:0.392 Z:0 W:1)  
        /// Color.Green     (R:0 G:128 B:0 A:255)    vector4 (X:0 Y:0.502 Z:0 W:1)  
        /// Color.OliveDrab (R:107 G:142 B:35 A:255) vector4 (X:0.420 Y:0.557 Z:0.137, W:1) 
        /// You can create colors with new Color(byte, byte, byte, byte) where byte = 0..255
        /// or, new Color(byte, byte, byte).
        /// 
        /// The Color conversion to Vector4 and back is used to add noise.
        /// You could just have Color.
        /// </summary>
        /// <returns>color texture</returns>

        private Texture2D createColorTexture()
        {

            //Local Variables
            float noiseTolerance = 10;
            Vector4 colorVec4;

            //Initializations
            textureMap1D = new Color[TEXTURE_WIDTH * TEXTURE_HEIGHT];
            colorMap = new Color[TEXTURE_WIDTH, TEXTURE_HEIGHT];
            colorVec4 = new Vector4();


            //Step through all the coords in the map and specify the color values
            for (int x = 0; x < TEXTURE_WIDTH; x++)
            {
                for(int z = 0; z < TEXTURE_HEIGHT; z++){

                    //Call a function which will decide on the color values
                    colorVec4 = heightToVector4(heightMap[x, z].R);

                    //Add some 'noise' or randomness to the color
                    colorVec4 = colorVec4 + new Vector4((float)(random.NextDouble() / noiseTolerance));

                    //Finally, convert to color and add to colorMap
                    colorMap[x, z] = new Color(colorVec4);
                }
            }


            //Convert colorMap[,] to textureMap1D[]
            int i = 0;
            for (int x = 0; x < TEXTURE_WIDTH; x++)
                for (int z = 0; z < TEXTURE_HEIGHT; z++)
                {
                    textureMap1D[i] = colorMap[x, z];
                    i++;
                }

            // create the texture to return.   
            Texture2D newTexture = new Texture2D(device, TEXTURE_WIDTH, TEXTURE_HEIGHT);
            newTexture.SetData<Color>(textureMap1D);
            return newTexture;
        }

        //####################################################################################################
        /// <summary>
        /// Process user keyboard input.
        /// Pressing 'T' or 't' will toggle the display between the height and color textures
        /// </summary>

        protected override void Update(GameTime gameTime)
        {
            KeyboardState keyboardState = Keyboard.GetState();
            if (keyboardState.IsKeyDown(Keys.Escape)) Exit();
            else if (Keyboard.GetState().IsKeyDown(Keys.T) && !oldState.IsKeyDown(Keys.T))
                showHeight = !showHeight;
            oldState = keyboardState;    // Update saved state.
            base.Update(gameTime);
        }

        //####################################################################################################
        /// <summary>
        /// Display the textures.
        /// </summary>
        /// <param name="gameTime"></param>

        protected override void Draw(GameTime gameTime)
        {
            device.Clear(ClearOptions.Target | ClearOptions.DepthBuffer, Color.White, 1, 0);
            spriteBatch.Begin();
            if (showHeight)  //This referes to the black and white map...
                spriteBatch.Draw(heightTexture, Vector2.Zero, Color.White);
            else //This refers to the color map
                spriteBatch.Draw(colorTexture, Vector2.Zero, Color.White);
            spriteBatch.End();

            base.Draw(gameTime);
        }

    }


}   