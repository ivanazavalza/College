/* Jared Fowler   yrolg4tseuq@sbcglobal.net
 * Project: P1 - Model Terrain and Treasures
 * Class: Comp565 with Prof. Barnes
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
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
    public class EyeCandy_Rocket : Model3D
    {
        //Variables---------------------------------------------------------------------------------------------
        //Void

        //Constructor-------------------------------------------------------------------------------------------
        public EyeCandy_Rocket(Stage theStage, string label, string meshFile, bool isCollidable = true)
            : base(theStage, label, meshFile, isCollidable)
        {
            //Local Variables
            UInt32 spacing = (UInt32)stage.Terrain.Spacing;
            Terrain terrain = stage.Terrain;

            UInt32[,] location = { { 450, 325 } };

            //Add the bricks to the stage
            for (UInt32 i = 0; i < location.GetLength(0); i++)
            {
                UInt32 xPos = location[i, 0];
                UInt32 zPos = location[i, 1];
                addObject(new Vector3(xPos * spacing, terrain.surfaceHeight((int)xPos, (int)zPos), zPos * spacing), Vector3.Up, (float)Math.PI, new Vector3(30, 30, 30));
            }
        }
    }
}