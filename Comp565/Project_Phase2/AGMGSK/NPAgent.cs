/* Jared Fowler   yrolg4tseuq@sbcglobal.net
 * Project: P1 - Model Terrain and Treasures
 * Class: Comp565 with Prof. Barnes
 * 
 *  Changes made by Jared Fowler include the following:
 *      -Commentary added throughout code for better understanding
 *      -Reorganized some function calls within update
 *      -Constructor changed to link to a treasure list
 *      -Variables treasreList and treasureGoal added
 *      -Variable treasureCount added along with get/set properties. Initialization in constructor
 *      -Variable treasureListNum added to keep track of which treasure in the list we are after
 *      -Function Update Modified to detect the 'n' key for treasure path. Also updated to handle treasure path finding and seeking.
 *      
 * Project: P2 - Navigation and Pack Behavoir
 *      -Added a check to see if NPAgent is within detection range of a treasure. Located in Update function.
 *      -Added a vairable called jwf_path which references a dictionary which contains the nodes used for A* algorithm path finding. ,
 *       as well as a call to the appropriate constructor. Added it to stage's drawable objects.
 *      -Updated the 'update' function to include a state called "returnPath". This state
 *       occurrs after the treasure has been found and NPAgent needs to return to navigation path.
 *      -Added keyboard check for keys 'j' and 'k'. These will enable/disable the visibility of all nodes on the map by settinga flag in
 *       class jwf_path.
 *      -Added A* algorithm function for treasure path finding. Other helper functions are also included at the bottom of this file.
 *   

 */

/*  
    Copyright (C) 2015 G. Michael Barnes   
  
    The file NPAgent.cs is part of AGMGSKv6 a port and update of AGXNASKv5 from
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
    /// A non-playing character that moves.  Override the inherited Update(GameTime)
    /// to implement a movement (strategy?) algorithm.
    /// Distribution NPAgent moves along an "exploration" path that is created by the
    /// from int[,] pathNode array.  The exploration path is traversed in a reverse path loop.
    /// Paths can also be specified in text files of Vector3 values, see alternate
    /// Path class constructors.
    /// 
    /// 12/31/2014 last changed
    /// </summary>
    public class NPAgent : Agent
    {
        //Constants
        private const UInt16 TREASURE_DETECT_RANGE = 4000;

        //Variables--------------------------------------------------------------------------------------
        private NavNode nextGoal;
        private Path path;
        private UInt32 snapDistance; //Distance to be considered "in reach" of goal object

        //**********************************************************
        private jwfPath jwf_path;  //Navigatable nodes for A*
        private Dictionary<string, PathFindingNode> closedList;
        private List<PathFindingNode> openList;
        private Stack<PathFindingNode> treasureRoute;

        private bool returnPath;

        //**********************************************************
        //Pointer to the treasure list
        private TreasureList treasureList;
        //Treasure goal
        private int treasureListNum;
        //Treasure Count
        private int treasureCount;
        //Treasure Goal boolean
        private bool treasurePath;

        public int treasure_count
        {
            get { return this.treasureCount; }
            set { this.treasureCount = value; }
        }

        //**********************************************************

        // If using makePath(int[,]) set WayPoint (x, z) vertex positions in the following array
        private int[,] pathNode = { {505, 490}, {500, 500}, {490, 505},  // bottom, right
                                    {435, 505}, {400,425}, {425, 500}, {420, 490},  // bottom, middle
                                    {420, 450}, {425, 440}, {435, 435},  // middle, middle
                                    {490, 435}, {500, 430}, {505, 420},  // middle, right
                                    {505, 105}, {500,  95}, {490,  90},  // top, right
                                    {110,  90}, {100,  95}, { 95, 105},  // top, left
                                    { 95, 480}, {100, 490}, {110, 495},  // bottom, left
                                    {320, 473}, {495, 480} };                        // loop return





        //Constructor------------------------------------------------------------------------------------
        /// <summary>
        /// Create a NPC. 
        /// AGXNASK distribution has npAgent move following a Path.
        /// </summary>
        /// <param name="theStage"> the world</param>
        /// <param name="label"> name of </param>
        /// <param name="pos"> initial position </param>
        /// <param name="orientAxis"> initial rotation axis</param>
        /// <param name="radians"> initial rotation</param>
        /// <param name="meshFile"> Direct X *.x Model in Contents directory </param>
        public NPAgent(Stage theStage, string label, Vector3 pos, Vector3 orientAxis, float radians, string meshFile, TreasureList tl, bool isCollidable = true)
            : base(theStage, label, pos, orientAxis, radians, meshFile)
        {  
            //Set variables
            this.isCollidable = isCollidable;
            this.treasureList = tl;
            this.treasureCount = 0;
            this.treasurePath = false;
            this.returnPath = false;

            if (this.isCollidable)
                stage.Collidable.Add(agentObject);

            //Change names for on-screen display of current camera
            first.Name = "npFirst";
            follow.Name = "npFollow";
            above.Name = "npAbove";

            // path is built to work on specific terrain, made from int[x,z] array pathNode
            path = new Path(stage, pathNode, Path.PathType.LOOP); // continuous search path
            stage.Components.Add(path);

            //Set up jwfNavigation Nodes
            jwf_path = new jwfPath(theStage, theStage.TerrainSize / 2, theStage.TerrainSize / 2, theStage.TerrainSize / 2 - 400);
            stage.Components.Add(jwf_path);

            //Get the first path goal
            nextGoal = path.NextNode;

            //Orient to face the first goal
            agentObject.turnToFace(nextGoal.Translation);

            //Set snapDistance to be a little larger than step * stepSize
            snapDistance = (UInt32)(1.5 * (agentObject.Step * agentObject.StepSize));
        }

        //Methods----------------------------------------------------------------------------------------
        /// <summary>
        /// Simple path following.  If within "snap distance" of a the nextGoal (a NavNode) 
        /// move to the NavNode, get a new nextGoal, turnToFace() that goal.  Otherwise 
        /// continue making steps towards the nextGoal.
        /// </summary>
        public override void Update(GameTime gameTime)
        {
            //Local Variables
            float distance, distance2;

            //Get the current keys being pushed on keyboard
            KeyboardState keyboardState = Keyboard.GetState();

            if(this.treasureList.TreasureRemaining == 0)
            {
                //Halt NPAgent
                this.instance[0].Step = 0;
                //Clean Up Previous Nodes
                cleanUpNodes();
            }
            //Are we on a treasure path?
            else if (this.treasurePath)
            {
                //Are we within grabbing range?
                distance = Vector2.Distance(new Vector2(agentObject.Translation.X, agentObject.Translation.Z),
                                            new Vector2(this.treasureList.getTreasureNode[this.treasureListNum].x * this.stage.Spacing, this.treasureList.getTreasureNode[this.treasureListNum].z * this.stage.Spacing));

                if (distance < (this.stage.Terrain.Spacing * 2))
                {
                    //We are within range! Collect the treasure and set the treasure as tagged... unless it has already been tagged
                    if (!this.treasureList.getTreasureNode[this.treasureListNum].isTagged)
                    {
                        this.treasureCount += 1;
                        this.treasureList.getTreasureNode[this.treasureListNum].isTagged = true;
                    }

                    //Change path to return route
                    this.treasurePath = false;
                    this.returnPath = true;

                    //Clean Up Previous Nodes
                    cleanUpNodes();

                    //Calculate A* path back to where the NPAgent originally came from
                    //Find the closest node to where we are at and where we are going.
                    PathFindingNode start, end;
                    end = findNearestTreasurePathNode((int)(this.nextGoal.Translation.X), (int)(this.nextGoal.Translation.Z));
                    start = findNearestTreasurePathNode((int)agentObject.Translation.X, (int)agentObject.Translation.Z);
                    aStar_PathFinder(start, end);

                    //Color the nodes
                    foreach (PathFindingNode p in openList)
                    {
                        p.nodeColor = Color.Green.ToVector3();
                        p.enabled = true;
                    }
                    foreach (KeyValuePair<string, PathFindingNode> p in closedList)
                    {
                        p.Value.nodeColor = Color.Black.ToVector3();
                        p.Value.enabled = true;
                    }
                    foreach (PathFindingNode p in treasureRoute)
                    {
                        p.nodeColor = Color.White.ToVector3();
                        p.enabled = true;
                    }
                    //Color destination a unique color
                    end.nodeColor = Color.HotPink.ToVector3();
                }
                else
                {
                    //Have we made it to the next treasure path node?
                    distance = Vector3.Distance(new Vector3(this.treasureRoute.Peek().x, 0, this.treasureRoute.Peek().z),
                                                 new Vector3(agentObject.Translation.X, 0, agentObject.Translation.Z)
                                               );

                    //Are we within grabbing range?
                    if (distance <= snapDistance)
                    {
                        //Pop node off stack
                        this.treasureRoute.Pop();
                    }


                    //Adjust the facing location towards the next treasure path node
                    if (this.treasureRoute.Count != 0)
                        agentObject.turnToFace(new Vector3(this.treasureRoute.Peek().x, 0, this.treasureRoute.Peek().z));
                }

                //Update the inspector pane
                stage.setInfo(15,
                   string.Format("npAvatar:  location ({0:f0}, {1:f0}, {2:f0})  looking at ({3:f2}, {4:f2}, {5:f2})",
                      agentObject.Translation.X, agentObject.Translation.Y, agentObject.Translation.Z,
                      agentObject.Forward.X, agentObject.Forward.Y, agentObject.Forward.Z));
                stage.setInfo(16,
                      string.Format("npAvatar:  treasureGoal ({0:f0}, {1:f0}, {2:f0})  distance to treasureGoal = {3,5:f2})",
                          nextGoal.Translation.X, nextGoal.Translation.Y, nextGoal.Translation.Z, distance));

                //if(this.treasureList.getTreasureNode.Length <= )


            }
            else if (this.returnPath)
            {
                //Have we made it to the next return path node?
                distance = Vector3.Distance(new Vector3(this.treasureRoute.Peek().x, 0, this.treasureRoute.Peek().z),
                                             new Vector3(agentObject.Translation.X, 0, agentObject.Translation.Z)
                                           );

                //Are we within grabbing range?
                if (distance <= snapDistance)
                {
                    //Pop node off stack
                    this.treasureRoute.Pop();

                    if (this.treasureRoute.Count == 0)
                    {
                        //Change path to normal navigation route
                        this.returnPath = false;
                        this.treasurePath = false;

                        //Clean Up Previous Nodes
                        cleanUpNodes();
                    }

                }
                else
                //Adjust the facing location towards the next treasure path node
                agentObject.turnToFace(new Vector3(this.treasureRoute.Peek().x, 0, this.treasureRoute.Peek().z));
                

            }

            else  //We are on the normal navigation node path
            {
                //Adjust the facing location towardes the next goal
                agentObject.turnToFace(nextGoal.Translation);

                //See if at or close to nextGoal, distance measured in 2D xz plane
                distance = Vector3.Distance(new Vector3(nextGoal.Translation.X, 0, nextGoal.Translation.Z),
                                             new Vector3(agentObject.Translation.X, 0, agentObject.Translation.Z)
                                           );

                //Are we within grabbing range?
                if (distance <= snapDistance)
                {
                    nextGoal = path.NextNode;
                }

                //Update the inspector pane
                stage.setInfo(15,
                   string.Format("npAvatar:  location ({0:f0}, {1:f0}, {2:f0})  looking at ({3:f2}, {4:f2}, {5:f2})",
                      agentObject.Translation.X, agentObject.Translation.Y, agentObject.Translation.Z,
                      agentObject.Forward.X, agentObject.Forward.Y, agentObject.Forward.Z));
                stage.setInfo(16,
                      string.Format("npAvatar:  nextGoal ({0:f0}, {1:f0}, {2:f0})  distance to next goal = {3,5:f2})",
                          nextGoal.Translation.X, nextGoal.Translation.Y, nextGoal.Translation.Z, distance));


                //Get the closest, un-tagged treasure
                distance = float.MaxValue;
                for (int i = 0; i < this.treasureList.getTreasureNode.Length; i++)
                {
                    if (this.treasureList.getTreasureNode[i].isTagged)
                        continue;
                        
                    distance2 = Vector2.Distance(new Vector2(agentObject.Translation.X, agentObject.Translation.Z),
                                                new Vector2(this.treasureList.getTreasureNode[i].x * this.stage.Spacing, this.treasureList.getTreasureNode[i].z * this.stage.Spacing));

                    if (distance2 < distance)
                    {
                        //Make the treasure the new goal
                        this.treasureListNum = i;
                        distance = distance2;
                    }
                    
                }

                //Is the NPAgent within sensing range of an untagged treasure?
                if (distance <= TREASURE_DETECT_RANGE || (keyboardState.IsKeyDown(Keys.N) && distance != float.MaxValue))
                {
                    //Do A* algorithm here and go after the treasure....
                    //Find the closest node to where we are at and where we are going.
                    PathFindingNode start, end;
                    end = findNearestTreasurePathNode((int)(this.treasureList.getTreasureNode[this.treasureListNum].x * stage.Spacing), (int)(this.treasureList.getTreasureNode[this.treasureListNum].z * stage.Spacing));
                    start = findNearestTreasurePathNode((int)agentObject.Translation.X, (int)agentObject.Translation.Z);
                    if (!aStar_PathFinder(start, end))
                        Console.Write("\nError. No Path Exists\n");

                    //Color the nodes
                    foreach (PathFindingNode p in openList)
                    {
                        p.nodeColor = Color.Green.ToVector3();
                        p.enabled = true;
                    }
                    foreach (KeyValuePair<string, PathFindingNode> p in closedList)
                    {
                        p.Value.nodeColor = Color.Black.ToVector3();
                        p.Value.enabled = true;
                    }
                    foreach (PathFindingNode p in treasureRoute)
                    {
                        p.nodeColor = Color.White.ToVector3();
                        p.enabled = true;
                    }
                    //Set destination node to special color
                    end.nodeColor = Color.HotPink.ToVector3();


                    this.treasurePath = true;
                }

            }

            //Show/Hide all nodes on map
            if (keyboardState.IsKeyDown(Keys.J))
            {
                this.jwf_path.showAllNodes = true;
            }
            else if (keyboardState.IsKeyDown(Keys.K))
            {
                this.jwf_path.showAllNodes = false;
            }
            

            base.Update(gameTime);
        }

        //Simple function which disables all nodes within open and closed lists
        void cleanUpNodes()
        {
            foreach (PathFindingNode p in openList)
            {
                p.enabled = false;
            }
            foreach (KeyValuePair<string, PathFindingNode> p in closedList)
            {
                p.Value.enabled = false;
            }
        }

        //Simple function which finds the nearest treasure path node to a specific coord
        PathFindingNode findNearestTreasurePathNode(int x, int z)
        {
            float dist = float.MaxValue;
            PathFindingNode retNode = null;

            foreach (KeyValuePair<string, PathFindingNode> navNode in this.jwf_path.pathNodeCollection)
            {
                if (Math.Abs(navNode.Value.x - x) + Math.Abs(navNode.Value.z - z) < dist)
                {
                    dist = Math.Abs(navNode.Value.x - x) + Math.Abs(navNode.Value.z - z);
                    retNode = navNode.Value;
                }
            }

            return retNode;
        }

        //A* algorithm by Jared Fowler. Fills in the open, closed, and path data structures.
        bool aStar_PathFinder(PathFindingNode start, PathFindingNode end)
        {
            //Initialize data structures
            this.closedList = new Dictionary<string, PathFindingNode>();
            this.openList = new List<PathFindingNode>();
            this.treasureRoute = new Stack<PathFindingNode>();

            //Have we reached the destination node?
            bool found = false;

            //Temporary waypoint
            PathFindingNode tempW = start;
            tempW.gValue = 0;
            tempW.ptr = null;
            //Add it to the open list
            openList.Add(tempW);

            //While the open list is not empty, || destination !Found
            while (openList.Count != 0 && !found)
            {
                //Deque from the open list
                openList.Sort();
                tempW = openList[0];
                openList.RemoveAt(0);
                openList.Sort();

                //Is this node the endpoint?
                if (string.Compare(tempW.id, end.id) == 0)
                {
                    found = true;
                    //Add to closed list
                    closedList.Add(tempW.id, tempW);
                    break;
                }
                //If not, put the node onto the closed list
                closedList.Add(tempW.id, tempW);
                //Add adjacent nodes to the open list
                foreach (PathFindingNode p in tempW.adjacent)
                {
                    //Is the node already on the closed list?
                    if (closedList.ContainsKey(p.id))
                        continue;

                    //Is this node already on the open list?
                    if (openList.Contains(p))
                        continue;

                    //Else
                    p.ptr = tempW; //Link this node back to former for path finding
                    p.gValue = tempW.gValue + p.distanceBetween(tempW) + p.distanceBetween(end);
                    openList.Add(p);
                }
            }

            //At this point we have either found a path or failed to do so....
            if(!found)
                return false;

            //Add destinatino node to the stack
            treasureRoute.Push(end);
            //Assign tempW to node that points to end node
            tempW = end.ptr;
            while (tempW != null)
            {
                //Add this node to stack
                treasureRoute.Push(tempW);
                //Re-assign tempW to obj that pointed to one just put on stack
                tempW = tempW.ptr;
            }

            return true;
        }
    }
}
