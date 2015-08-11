/* Jared Fowler   yrolg4tseuq@sbcglobal.net
 * Project: P1 - Model Terrain and Treasures
 * Class: Comp565 with Prof. Barnes
 * 
 *  Changes made by Jared Fowler include the following:
 *      -Commentary added throughout code for better understanding
 *      -Some int types changed to UInt32
 *      -Variables initialization taken out of loop for optimization
 *      
 * Project: P2 - Navigation and Pack Behavoir
 *      -Removed unused parameter for contructor. nDogs
 *      -Manually added more dogs and positions of them.
 *      -Added three variables which determine the probability of packing and cohesion factor
 *      -Re-wrote the update function to handle packing. There are mainly 2 calcuations that take place,
 *       the first pertaining to Cohesion and Alighnment, and the second dealing with Separation.
 * 
 */

/*  
    Copyright (C) 2015 G. Michael Barnes
  
 *  Changes made by Jared Fowler include the following:
 *      -Commentary added throughout code for better understanding
 *      -Some int types changed to UInt32
 *      -Variables initialization taken out of loop for optimization
 
    The file Pack.cs is part of AGMGSKv6 a port and update of AGXNASKv5 from
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
    /// Pack represents a "flock" of MovableObject3D's Object3Ds.
    /// Usually the "player" is the leader and is set in the Stage's LoadContent().
    /// With no leader, determine a "virtual leader" from the flock's members.
    /// Model3D's inherited List<Object3D> instance holds all members of the pack.
    /// 
    /// 2/1/2015 last changed
    /// </summary>
    public class Pack : MovableModel3D
    {
        //Variables--------------------------------------------------------------------------
        Object3D leader;

        public readonly int[] packingProbabilities = new int[] { 0, 33, 66, 99 };
        int packingProbability = 0;
        const int leaderCohesionRadius = 2500;
        const int leaderSeparationRadius = 1500;
        KeyboardState keyboardState, oldKeyboardState;

        public int PackingProbability
        {
            get { return packingProbabilities[this.packingProbability]; }
        }

        //Constructor------------------------------------------------------------------------
        /// <summary>
        /// Construct a pack with an Object3D leader
        /// </summary>
        /// <param name="theStage"> the scene     </param>
        /// <param name="label">    name of pack  </param>
        /// <param name="meshFile"> model of a pack instance</param>
        /// <param name="xPos, zPos">  approximate position of the pack </param>
        /// <param name="aLeader"> alpha dog can be used for flock center and alignment </param>
        public Pack(Stage theStage, string label, string meshFile, int xPos, int zPos, Object3D theLeader, bool isCollidable = true)
            : base(theStage, label, meshFile)
        {
            this.isCollidable = isCollidable;
            this.random = new Random();
            this.leader = theLeader;

            
            //Local Variables
            UInt32 spacing = (UInt32)stage.Spacing;
            Int32 x, z;
            float scale;

            //Initial vertex offset of dogs around (xPos, zPos)
            Int32[,] position = { { 0, 0 }, { 10, -10 }, { -10, -10 }, { -10, 10 }, { 10, 10 }, {15, 15}, {-15, -15},  {-15, 15}};

            for (int i = 0; i < position.GetLength(0); i++)
            {
                //Position the dogs
                x = xPos + position[i, 0];
                z = zPos + position[i, 1];

                //Scale the dogs
                scale = (float)(0.5 + random.NextDouble());

                //Add the pack member to the instance list
                addObject(new Vector3(x * spacing, stage.surfaceHeight(x, z), z * spacing),
                              new Vector3(0, 1, 0), 
                              0.0f,
                              new Vector3(scale, scale, scale));
            }
        }

        //-----------------------------------------------------------------------------------
        /// <summary>
        /// Each pack member's orientation matrix will be updated.
        /// Distribution has pack of dogs moving randomly.  
        /// Supports leaderless and leader based "flocking" 
        /// </summary>      
        public override void Update(GameTime gameTime)
        {
            //Get the current keys being pushed on keyboard
            keyboardState = Keyboard.GetState();

            //Determine if user wants to change the packing probability percentage...
            if (keyboardState.IsKeyDown(Keys.P) && !oldKeyboardState.IsKeyDown(Keys.P))
            {
                this.packingProbability += 1;
                if (this.packingProbability >= packingProbabilities.Length)
                    this.packingProbability = 0;
            }

            //Update the old keyboard state for next time.
            oldKeyboardState = keyboardState;

            float avgX = 0, avgZ = 0;
            //Get average location of all dogs
            foreach (Object3D obj in instance)
            {
                avgX += obj.Translation.X;
                avgZ += obj.Translation.Z;
            }

            avgX /= this.instance.Count;
            avgZ /= this.instance.Count;

            /*JWF - P2 shall have a flocking algorithm derived here in this function.
             * This includes three aspects, namely: Cohesion, Separation, and Alighnment. 
             * Both Cohesion and alighnment will be based upon the leaders current position.
             * Separation will be based upon both leader position and other pack object positions.*/
            int randNum;
            float Iangle;
            float angle;
            foreach (Object3D obj in instance)
            {
                obj.Yaw = 0.0f;
                randNum = random.Next(1,100);

                if (randNum <= packingProbabilities[this.packingProbability])
                {
                    Vector2 playerAt = Vector2.Normalize(new Vector2(this.leader.Forward.X, this.leader.Forward.Z));
                    Vector2 dogAt = Vector2.Normalize(new Vector2(obj.Forward.X, obj.Forward.Z));
                    Vector2 playerPos = new Vector2(this.leader.Translation.X, this.leader.Translation.Z);
                    Vector2 dogPos = new Vector2(obj.Translation.X, obj.Translation.Z);
                    Vector2 chaseVec = Vector2.Normalize(new Vector2(playerPos.X - dogPos.X, playerPos.Y - dogPos.Y));


                    /*******************************Packing*************************************************/
                    /*Packing is based off 3 main forces: Cohesion, Separation, and Alighnment. I will base 
                     * my algorithm off a three tier area, where in each teir one of the forces will have the
                     * greatest effect. Cohesion will have the greatest effect in the outer most sphere, alighnment
                     * in the middle, and separation in the inner-most. These forces will be represented by Vector3's
                     * which will simply be added together, and then the normal of this vector will be taken to calculate
                     * the angle of yaw rotation.
                     */

                    Vector2 cohesionForce = new Vector2(0);
                    Vector2 separationForce = new Vector2(0);
                    Vector2 alignmentForce = new Vector2(0);

                    //Are we at a strong cohesion point?
                    if (Vector2.Distance(playerPos, dogPos) > leaderCohesionRadius)
                    {
                        cohesionForce += (chaseVec * random.Next(3,5));
                        separationForce += (-chaseVec) * random.Next(1,2);
                        alignmentForce += (playerAt * random.Next(1,2));
                    }
                    //Are we at a strong separation point?
                    else if (Vector2.Distance(playerPos, dogPos) < leaderSeparationRadius)
                    {
                        cohesionForce += (chaseVec) * random.Next(1,2);
                        separationForce += (-chaseVec) * random.Next(3,5);
                        alignmentForce += (playerAt) * random.Next(1,2);
                    }
                    //Are we at a strong alignment point?
                    else
                    {
                        cohesionForce += (chaseVec) * random.Next(1,2);
                        separationForce += (-chaseVec) * random.Next(1,2);
                        alignmentForce += (playerAt * random.Next(1,packingProbability + 1));
                    }

                    //Get the sum of these vector forces and then normalize
                    Vector2 forceSum = new Vector2();
                    forceSum = Vector2.Normalize(cohesionForce + separationForce + alignmentForce);

                    //Ge the angle between the forceSum and the dog's current at vector.. this will be the yaw
                    Iangle = (float)(Vector2.Dot(dogAt, forceSum));
                    if (Math.Abs(Iangle) > 1)
                        Iangle = 1;
                    angle = (float)Math.Acos(Iangle);
                    obj.Yaw = (angle * .1f);

                }
                else
                {
                    /*Random Behavior*/
                    angle = 0.3f;
                    obj.Yaw = 0.0f;

                    // change direction 4 times a second  0.07 = 4/60
                    if (random.NextDouble() < 0.07)
                    {
                        if (random.NextDouble() < 0.5)
                            obj.Yaw -= angle; // turn left
                        else
                            obj.Yaw += angle; // turn right
                    }
                }

                //Update the object
                obj.updateMovableObject();
                stage.setSurfaceHeight(obj);
            }

            base.Update(gameTime);  // MovableMesh's Update(); 
        }

        //-----------------------------------------------------------------------------------
        public Object3D Leader
        {
            get { return leader; }
            set { leader = value; }
        }

    }
}
