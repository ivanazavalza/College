/* Jared Fowler   yrolg4tseuq@sbcglobal.net
 * Project: P1 - Model Terrain and Treasures
 * Class: Comp565 with Prof. Barnes
 * 
 * 
 * Project 2:
 *   --Added a fifth treasure to the map as to avoid ties, and repositioned
 *    them to be close to the navigation nodes for path-finding.
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
    public struct TreasureNode
    {
        public float x;
        public float z;
        public bool isTagged;

        public TreasureNode(float x, float z, bool isTagged)
        {
            this.x = x;
            this.z = z;
            this.isTagged = isTagged;
        }
    }

    /// <summary>
    /// This class holds information regarding a lsit of treasure nodes. A treasure
    /// will be characterized by its location, and whether it has been taken or not.
    /// Once a treasure has been taken, it is first tagged, and then a visual marker
    /// should be placed to indicate such.
    /// </summary>
    /// 
    public class TreasureList : Model3D
    {
        //Variables
        private TreasureNode[] treasureNode;
        private int treasuresRemaining;

        public int TreasureRemaining
        {
            get { return this.treasuresRemaining;}
        }
        
        public TreasureList(Stage theStage, string label, string meshFile, bool isCollidable = false)
            : base(theStage, label, meshFile)
        {
            this.isCollidable = isCollidable;
            
            //The following represents a list of treausre locations..
            int[,] treasure = {
                                  {393,400}, 
                                  {509,493},  
                                  {320,460}, 
                                  {447,453}, 
                                  {320,493}
                              };
            this.treasuresRemaining = treasure.Length;

            //Create a list of treasures
            this.treasureNode = new TreasureNode[treasure.GetLength(0)];
            int x, z;

            for (int i = 0; i < treasure.GetLength(0); i++)
            {
                //Positions
                x = treasure[i, 0];
                z = treasure[i, 1];

                //Keep a list of these locations
                this.treasureNode[i].x = x;
                this.treasureNode[i].z = z;
                this.treasureNode[i].isTagged = false;

                //Add the treasure
                addObject(new Vector3(x * stage.Spacing, stage.Terrain.surfaceHeight(x, z), z * stage.Spacing),
                          new Vector3(0, 1, 0),
                          0.0f,
                          new Vector3(1, 1, 1));
                         
            }
            

        }

        public TreasureNode[] getTreasureNode
        {
            get { return this.treasureNode; }
        }


        public override void Update(GameTime gameTime)
        {
            //Re-count number of remaining treasures
            treasuresRemaining = 0;

            for(int i = 0; i < this.instance.Count; i++){

                this.instance[i].Step = 0;

                //If the treasure is not tagged, spin it
                if(!this.treasureNode[i].isTagged)
                {
                    this.instance[i].Yaw = .01f;
                    this.instance[i].updateMovableObject();
                    treasuresRemaining += 1;
                }
                else
                {
                    this.instance[i].Yaw = 0.0f;
                    this.instance[i].updateMovableObject();
                }

            }

            base.Update(gameTime);
        }

    }



}