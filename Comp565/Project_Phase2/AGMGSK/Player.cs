/* Jared Fowler   yrolg4tseuq@sbcglobal.net
 * Project: P1 - Model Terrain and Treasures
 * Class: Comp565 with Prof. Barnes
 * 
 * 
 *  Changes made by Jared Fowler include the following:
 *      -Commentary added throughout code for better understanding
 *      -Collidable parameter added to constructor
 *      -If condition added for stage collidable list
 *      -Added a link reference to treasure list
 *      -Added a integer to keep track of treasure count
 *      -Treasure count set to 0 in constructor and get/set method added
 *      -Update function has been updated with collision detection with treasures
 * 

 */

/*  
    Copyright (C) 2015 G. Michael Barnes

    The file Player.cs is part of AGMGSKv6 a port and update of AGXNASKv5 from
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
    /// Represents the user / player interacting with the stage. 
    /// The Update(Gametime) handles both user keyboard and gamepad controller input.
    /// If there is a gamepad attached the keyboard inputs are not processed.
    /// 
    /// removed game controller code from Update()
    /// 
    /// 2/8/2014 last changed
    /// </summary>


    public class Player : Agent
    {
        //Variables--------------------------------------------------------------------------
        private KeyboardState oldKeyboardState;
        private Matrix initialOrientation;  //This is kept track of so that we can restart to our original spawn
        private Int32 rotate;
        private float angle;

        private TreasureList treasreList;
        private int treasureCount;

        public int treasure_count
        {
            get { return this.treasureCount; }
            set { this.treasureCount = value; }
        }

        
        //Constructor------------------------------------------------------------------------
        public Player(Stage theStage, string label, Vector3 pos, Vector3 orientAxis, float radians, string meshFile, TreasureList tl, bool isCollidable = true)
            : base(theStage, label, pos, orientAxis, radians, meshFile)
        {  
            //Camera Name ID's
            first.Name = "First";
            follow.Name = "Follow";
            above.Name = "Above";

            //Set if this object is collidable
            this.IsCollidable = isCollidable;
            if(this.isCollidable)
                stage.Collidable.Add(agentObject);


            this.treasreList = tl;
            this.treasureCount = 0;


            //Set initial orientation of player
            this.rotate = 0;
            this.angle = 0.01f;
            initialOrientation = agentObject.Orientation;
        }

        //-----------------------------------------------------------------------------------
        /// <summary>
        /// Handle player input that affects the player.
        /// See Stage.Update(...) for handling user input that affects
        /// how the stage is rendered.
        /// First check if gamepad is connected, if true use gamepad
        /// otherwise assume and use keyboard.
        /// </summary>
        /// <param name="gameTime"></param>
        public override void Update(GameTime gameTime)
        {

            //Treasure Collision: Have we come within 300 pixels of an untagged treasure?
            int thisPosX = (int)this.instance[0].Translation.X;
            int thisPosZ = (int)this.instance[0].Translation.Z;
            float distance;

            for (int i = 0; i < treasreList.Instance.Count; i++)
            {
                int x, z;

                //Is this treasure tagged?
                if (this.treasreList.getTreasureNode[i].isTagged)
                {
                    continue;
                }
                
                //Else
                //Load in the position of the treasure
                x = (int)this.treasreList.getTreasureNode[i].x * this.stage.Terrain.Spacing;
                z = (int)this.treasreList.getTreasureNode[i].z * this.stage.Terrain.Spacing;

                //Are we within 300 pixels of the treasure?
                distance = Vector2.Distance(new Vector2(x,z), new Vector2(thisPosX, thisPosZ));

                if (distance < (this.stage.Terrain.Spacing * 2))
                {
                    //We are within range! Collect the treasure and set the treasure as tagged.
                    this.treasureCount += 1;
                    this.treasreList.getTreasureNode[i].isTagged = true;

                }



            }

            //Get the current keys being pushed on keyboard
            KeyboardState keyboardState = Keyboard.GetState();

            //Should we restart the player from its initial spawn?
            if (keyboardState.IsKeyDown(Keys.R) && !oldKeyboardState.IsKeyDown(Keys.R))
                agentObject.Orientation = initialOrientation;

            //Update yaw and step variables for future movement calculation
            if (keyboardState.IsKeyDown(Keys.Up)) 
                agentObject.Step++;
            if (keyboardState.IsKeyDown(Keys.Down)) 
                agentObject.Step--;
            if (keyboardState.IsKeyDown(Keys.Left)) 
                rotate++;
            if (keyboardState.IsKeyDown(Keys.Right)) 
                rotate--;

            //Back-up this keyboard state
            oldKeyboardState = keyboardState;

            //Update the players position
            agentObject.Yaw = rotate * angle;
            base.Update(gameTime);

            //Reset the movement variables, lest we can't stop ourselves!
            rotate = agentObject.Step = 0;
        }
    }
}