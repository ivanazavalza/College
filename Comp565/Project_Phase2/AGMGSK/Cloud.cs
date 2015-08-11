/* Jared Fowler   yrolg4tseuq@sbcglobal.net
 * Project: P1 - Model Terrain and Treasures
 * Class: Comp565 with Prof. Barnes
 * 
 *  Changes made by Jared Fowler include the following:
 *      -Commentary added throughout code for better understanding
 *      -Some int types changed to UInt32
 *      -Fixed cloud scale in correspondance with making it into a space ship
 *      -Increased height of cloud
 *      -Fixed rotation radians of cloud
 * 
 * 
 * 
 */

/*  
    Copyright (C) 2015 G. Michael Barnes

    The file Cloud.cs is part of AGMGSKv6 a port and update of AGXNASKv5 from
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
    /// An example of how to override the MovableModel3D's Update(GameTime) to 
    /// animate a model's objects.  The actual update of values is done by calling 
    /// each instance object and setting its (Pitch, Yaw, Roll, or Step property. 
    /// Then call base.Update(GameTime) method of MovableModel3D to apply transformations.
    /// 
    /// 1/5/2014  last changed
    /// </summary>
    public class Cloud : MovableModel3D
    {
        // Constructor
        public Cloud(Stage stage, string label, string meshFile, int nClouds)
            : base(stage, label, meshFile)
        {
            random = new Random();

            //Add nClouds random cloud instances
            for (UInt32 i = 0; i < nClouds; i++)
            {
                UInt32 x = (UInt32)((128 + random.Next(256)) * stage.Spacing);
                UInt32 z = (UInt32)((128 + random.Next(256)) * stage.Spacing);

                addObject(
                    new Vector3(x, stage.surfaceHeight(x, z) + 15000, z),
                    new Vector3(0, 1, 0), 4.0f,
                    new Vector3(15, 4, 15));
            }
        }

        public override void Update(GameTime gameTime)
        {
            foreach (Object3D obj in instance)
            {
                obj.Step = 0;
                obj.Yaw = 0;

                //Determine spinning speed
                if (random.Next(3) == 0) 
                    obj.Yaw = 20 * 0.001f;

                obj.updateMovableObject();
            }
            base.Update(gameTime);
        }

    }
}