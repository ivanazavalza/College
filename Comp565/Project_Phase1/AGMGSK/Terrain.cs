/* Jared Fowler   yrolg4tseuq@sbcglobal.net
 * Project: P1 - Model Terrain and Treasures
 * Class: Comp565 with Prof. Barnes
 * 
 * 
 *  Changes made by Jared Fowler include the following:
 *      -Commentary added throughout code for better understanding
 *      -Some type int were changed to UInt32  
 *      -Deleted unused constructor (Just to not confuse me with more code)
 *      -Changed fog to black
 * 
 */

/*  
    Copyright (C) 2015 G. Michael Barnes
 
    The file Terrain.cs is part of AGMGSKv6 a port and update of AGXNASKv5 from
    XNA 4 refresh to MonoGames 3.2.  

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

#region Using Statements
using System;
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
#if MONOGAMES //  true, build for MonoGames
   using Microsoft.Xna.Framework.Storage; 
#endif
#endregion

namespace AGMGSKv6
{

    /// <summary>
    /// Terrain represents a ground.
    /// The vertices have position and color.  Terrain width = height.  
    /// Reads two textures to set terrain height and color values.
    /// You might want to pre-compute and store heights of surfaces to be 
    /// returned by the surfaceHeight(x, z) method.
    /// 
    /// 1/25/2012 last changed
    /// </summary>
    /// 
    public class Terrain : IndexVertexBuffers
    {
        //Constants--------------------------------------------------------------------------------------
        private readonly UInt32 multiplier = 40; //Multiplied by all height values to add variance...

        //Variables--------------------------------------------------------------------------------------
        private BasicEffect    effect;
        private GraphicsDevice display;

        protected VertexPositionColor[] vertex;  // stage vertices    
        private UInt32[,] terrainHeight;

        private UInt32 height     = 0;
        private UInt32 width      = 0;
        private UInt32 spacing    = 0;

        //Constructor------------------------------------------------------------------------------------
        /// <summary>
        /// Make a Terrain from 2 png texture files.  
        /// Must have __XNA4__ capabilities to read from png files.
        /// </summary>
        /// <param name="theStage"></param>
        /// <param name="label"></param>
        /// <param name="heightFile"></param>
        /// <param name="colorFile"></param>
        public Terrain(Stage theStage, string label, string heightFile, string colorFile)
            : base(theStage, label)
        {
            //Local Variables
            UInt32 i = 0;
            UInt32 vertexHeight;
            Vector4   vector4;
            Texture2D heightTexture;
            Texture2D colorTexture;
            Microsoft.Xna.Framework.Color[] heightMap;
            Microsoft.Xna.Framework.Color[] colorMap;

            //Initialization of most base class variables (IndexVertexBuffers)
            constructorInit();

            //Load in height map file and load up height texture
            heightTexture = stage.Content.Load<Texture2D>(heightFile);
            heightMap = new Microsoft.Xna.Framework.Color[width * height];
            heightTexture.GetData<Microsoft.Xna.Framework.Color>(heightMap);

            //Load in color map file and load up color texture
            colorTexture = stage.Content.Load<Texture2D>(colorFile);
            colorMap = new Microsoft.Xna.Framework.Color[width * height];
            colorTexture.GetData<Microsoft.Xna.Framework.Color>(colorMap);

            //Create vertices for the plain
            for (UInt32 z = 0; z < height; z++)
            {
                for (UInt32 x = 0; x < width; x++)
                {
                    //Convert the images RBGA values into a vec4
                    vector4 = heightMap[i].ToVector4();       

                    //Scale these value into a height value (Remember, R = G = B = A)
                    vertexHeight = (UInt32)(vector4.X * 255);

                    //To make things a bit more interesting, mulitply the height by a const value
                    vertexHeight *= multiplier;

                    //Save this height for navigation
                    terrainHeight[x, z] = vertexHeight;

                    //Back up the color of the vertex with its coordinance
                    vertex[i] = new VertexPositionColor(new Vector3(x * spacing, vertexHeight, z * spacing),
                                                        new Color(colorMap[i].ToVector4()));

                    i++;
                }
            }
                
            //Free up the now uneeded maps
            colorMap  = null;
            heightMap = null;

            //Fill in our indices, which make up our triangulated mesh pieces
            makeIndicesSetData();
        }

        //Methods----------------------------------------------------------------------------------------
        private void constructorInit()
        {
            //Allocate space for terrain height map
            range = (UInt32)stage.Range;
            width = height = range;
            terrainHeight = new UInt32[width, height];

            //Allocate space for vertices
            nVertices = width * height;
            vertex = new VertexPositionColor[nVertices];

            //Allocate space for indices
            nIndices = (width - 1) * (height - 1) * 6;
            indices = new UInt32[nIndices];

            //Get a copy of the spacing value from stage
            spacing = (UInt32)stage.Spacing;

            // set display information 
            display = stage.Display;
            effect  = stage.SceneEffect;
        }

        //-----------------------------------------------------------------------------------------------
        private void makeIndicesSetData()
        {
            //Set indices clockwise from point of view ... surfaces really left handed
            UInt32 i = 0;
            for (UInt32 z = 0; z < height - 1; z++)
                for (UInt32 x = 0; x < width - 1; x++)
                {
                    indices[i++] = z * width + x;
                    indices[i++] = z * width + x + 1;
                    indices[i++] = (z + 1) * width + x;
                    indices[i++] = (z + 1) * width + x;
                    indices[i++] = z * width + x + 1;
                    indices[i++] = (z + 1) * width + x + 1;
                }

            // create VertexBuffer and store on GPU
            vb = new VertexBuffer(display, typeof(VertexPositionColor), vertex.Length, BufferUsage.WriteOnly);
            vb.SetData<VertexPositionColor>(vertex);

            // create IndexBuffer and store on GPU
            ib = new IndexBuffer(display, typeof(int), indices.Length, BufferUsage.WriteOnly);
            IB.SetData<UInt32>(indices);
        }

        //-----------------------------------------------------------------------------------------------
        public int Spacing
        {
            get { return stage.Spacing; }
        }

        //-----------------------------------------------------------------------------------------------
        ///<summary>
        /// Height of  surface containing position (x,z) terrain coordinates.
        /// This method is a "stub" for the correct get code.
        /// How would you determine a surface's height at (x,?,z) ? 
        /// </summary>
        /// <param name="x"> left -- right terrain position </param>
        /// <param name="z"> forward -- backward terrain position</param>
        /// <returns> vertical height of surface containing position (x,z)</returns>
        public float surfaceHeight(int x, int z)
        {
            if (x < 0 || x > 511 || z < 0 || z > 511) 
                return 0.0f;  // index valid ?

            return (float)terrainHeight[x, z];
        }

        //-----------------------------------------------------------------------------------------------
        public override void Draw(GameTime gameTime)
        {
            effect.VertexColorEnabled = true;

            if (stage.Fog)
            {
                effect.FogColor = Color.Black.ToVector3();
                effect.FogStart = stage.FogStart;
                effect.FogEnd = stage.FogEnd;
                effect.FogEnabled = true;
            }
            else 
                effect.FogEnabled = false;

            effect.DirectionalLight0.DiffuseColor = stage.DiffuseLight;
            effect.AmbientLightColor = stage.AmbientLight;
            effect.DirectionalLight0.Direction = stage.LightDirection;
            effect.DirectionalLight0.Enabled = true;
            effect.View = stage.View;
            effect.Projection = stage.Projection;
            effect.World = Matrix.Identity;

            foreach (EffectPass pass in effect.CurrentTechnique.Passes)
            {
                pass.Apply();
                display.SetVertexBuffer(vb);
                display.Indices = ib;
                display.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, (int)nVertices, 0, (int)(nIndices / 3));
            }
        }
    }
}
