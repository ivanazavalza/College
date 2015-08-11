/* Jared Fowler   yrolg4tseuq@sbcglobal.net
 * Project: P2
 * Class: Comp565 with Prof. Barnes
 * 
 *  This file contains the tools necessary to create and store treasure navigation waypoint nodes. 
 *  A graph will be generated based upon min and max node separation distances and the existence of 
 *  collidable objects in the terrain. These nodes will be stored in a C# Dictionary, with the key
 *  being the combination of x,z coord separated with '::'.  The A* function can be found in NPAgent.
 * 
 */

#region Using Statements
using System;
using System.Linq;
using System.Text;
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
   /*Simple node which represents waypoints used for the A* algorithm for treasure path finding.*/
    public class PathFindingNode : IComparable
    {
        private string _id;
        private Int32 _x, _y, _z;
        private Vector3 _nodeColor;
        private bool _enabled = false;
        private LinkedList<PathFindingNode> _adjacent;
        private PathFindingNode _ptr = null;
        private float _gValue = 0;

        public string id
        {
            get { return this._id; }
            set { this._id = value; }
        }
        public Int32 x
        {
            get { return this._x; }
            set { this._x = value; }
        }
        public Int32 y
        {
            get { return this._y; }
            set { this._y = value; }
        }
        public Int32 z
        {
            get { return this._z; }
            set { this._z = value; }
        }
        public Vector3 nodeColor
        {
            get { return this._nodeColor; }
            set { this._nodeColor = value; }
        }
        public bool enabled
        {
            get { return this._enabled; }
            set { this._enabled = value; }
        }
        public LinkedList<PathFindingNode> adjacent
        {
            get { return this._adjacent; }
            set { this._adjacent = value; }
        }
        public PathFindingNode ptr
        {
            get { return this._ptr; }
            set { this._ptr = value; }
        }
        public float gValue
        {
            get { return this._gValue; }
            set { this._gValue = value; }
        }

        public PathFindingNode(Stage theStage, Int32 x, Int32 z)
        {
            this.nodeColor = Color.Red.ToVector3();
            this.x = x;
            this.z = z;
            this.y = (Int32)theStage.surfaceHeight((float)x, (float)z);
            this.id = string.Format("{0}::{1}", x, z);
            adjacent = new LinkedList<PathFindingNode>();
        }

        public bool addToLL(PathFindingNode p)
        {
            //Don't allow node to point to itself
            if (string.Compare(p.id, this.id) == 0)
                return false;

            //We'll want to manually check containment based upon id.
            PathFindingNode[] pAry = this.adjacent.ToArray<PathFindingNode>();

            for (int i = 0; i < pAry.Length; i++)
            {
                if (string.Compare(pAry[i].id, p.id) == 0)
                    return false;

                //We also want to avoid nodes that pretty much overlap...
                //if (Math.Abs(p.x - pAry[i].x) <= 5 && Math.Abs(p.z - pAry[i].z) <= 5)
                 //   return false;

            }

            //Else, add to adjacency list
            adjacent.AddLast(p);
            return true;
        }

        public float distanceBetween(PathFindingNode p)
        {
            /*Finds the distance between this node and the one passed in*/
            return (float)(Math.Sqrt(Math.Pow(this.x - p.x, 2) + Math.Pow(this.z - p.z, 2)));
        }

        public int CompareTo(Object obj)
        {
            if (obj == null)
                return 1;

            PathFindingNode p = obj as PathFindingNode;

            if (p.gValue == this.gValue)
                return 0;
            else if (p.gValue < this.gValue)
                return 1;
            else
                return -1;
        }
    }

    /*Class in charge of generating and storing waypoint nodes which represent treasure waypoints for the NPAgent*/
    public class jwfPath : DrawableGameComponent
    {
        //Constants
        private const UInt32 MAX_NODE_SEPARATION = 3750;
        private const UInt32 MIN_NODE_SEPARATION = 150;

        //Variables
        private Dictionary<string, PathFindingNode> _pathNodeCollection;
        private Stage _stage;

        private bool _showAllNodes = false;

        public Dictionary<string, PathFindingNode> pathNodeCollection
        {
            get { return this._pathNodeCollection; }
            set { this._pathNodeCollection = value; }
        }
        public Stage stage
        {
            get { return this._stage; }
            set { this._stage = value; }
        }
        public bool showAllNodes
        {
            get { return this._showAllNodes; }
            set { this._showAllNodes = value; }
        }

        public jwfPath(Stage theStage, Int32 startX, Int32 startZ, Int32 dist)
            :base(theStage)
        {
            this.stage = theStage;
            setUpNavigationGraph(startX, startZ, dist);
        }

        public bool jwf_QuadTree(PathFindingNode node, Int32 dist)
        {
            //Given that 'node' is located in the middle of the quad 
            //Determine if we should split it into 4 smaller quads based upon
            //1. If the distance is greater than the maximum allowed distance
            //2. There is a colidable object within the quad
            if ((dist > MAX_NODE_SEPARATION) || jwf_existsCollidable(node, dist))
            {
                //Are the nodes already at the minimum distance?
                if (dist <= MIN_NODE_SEPARATION)
                {
                    //We are done.
                    return true;
                }

                //Else, recursively call this funciton with 4 new quads
                //Upper left quad
                jwf_QuadTree(new PathFindingNode(this.stage, node.x - (Int32)(dist / 2), node.z - (Int32)(dist / 2)), dist / 2);
                //Upper right quad
                jwf_QuadTree(new PathFindingNode(this.stage, node.x + (Int32)(dist / 2), node.z - (Int32)(dist / 2)), dist / 2);
                //Lower left quad
                jwf_QuadTree(new PathFindingNode(this.stage, node.x - (Int32)(dist / 2), node.z + (Int32)(dist / 2)), dist / 2);
                //Lower right quad
                jwf_QuadTree(new PathFindingNode(this.stage, node.x + (Int32)(dist / 2), node.z + (Int32)(dist / 2)), dist / 2);
            }
            else
            {
                //Set up 9 nodes
                PathFindingNode[] ary = new PathFindingNode[]{ 
                new PathFindingNode(this.stage, node.x - dist, node.z - dist),
                new PathFindingNode(this.stage, node.x, node.z - dist),
                new PathFindingNode(this.stage, node.x + dist, node.z - dist),
                new PathFindingNode(this.stage, node.x - dist, node.z),
                node,
                new PathFindingNode(this.stage, node.x + dist, node.z),
                new PathFindingNode(this.stage, node.x - dist, node.z + dist),
                new PathFindingNode(this.stage, node.x, node.z + dist),
                new PathFindingNode(this.stage, node.x + dist, node.z + dist),
                };

                //Add nodes to dictionary
                for (int i = 0; i < ary.Length; i++)
                {
                    ary[i] = checkContains_andAdd_jwf(ary[i]);
                }

                //Add links 
                for (int i = 0; i < ary.Length; i++)
                {
                    //Every node (except for center node) gets linked to center node.
                    this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[4].id]);

                    switch (i)
                    {
                        case 0: //upper left
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[1].id]);
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[3].id]);
                            break;
                        case 1: //upper middle
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[0].id]);
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[2].id]);
                            break;
                        case 2: //upper right
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[1].id]);
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[5].id]);
                            break;
                        case 3: //middle left
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[0].id]);
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[6].id]);
                            break;
                        case 4: //middle middle
                            for (int j = 0; j < ary.Length; j++)
                                this.pathNodeCollection[node.id].addToLL(this.pathNodeCollection[ary[j].id]);
                            break;
                        case 5: //middle right
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[2].id]);
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[8].id]);
                            break;
                        case 6: //lower left
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[3].id]);
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[7].id]);
                            break;
                        case 7: //lower middle
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[6].id]);
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[8].id]);
                            break;
                        case 8: //lower right
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[7].id]);
                            this.pathNodeCollection[ary[i].id].addToLL(this.pathNodeCollection[ary[5].id]);
                            break;
                        default:
                            Console.Write("\nIllegal Node Link Error...\n");
                            break;
                    }
                }
            }

            return true;
        }

        PathFindingNode checkContains_andAdd_jwf(PathFindingNode p)
        {
            /*In order to avoid nodes positioned almost directly on top of one-another,
             * this function will be a helper to quad-tree. If the node already exists in the
             * dictionary, or a node close enough to that node, then this function will return
             * a reference to that node, else, it returns passed in value */

            //Check for close by nodes, including the percise node
            for (int i = -25; i <= 25; i++)
                for (int j = -25; j <= 25; j++)
                {
                    if (this.pathNodeCollection.ContainsKey(string.Format("{0}::{1}", p.x + i, p.z + j)))
                    {
                        return this.pathNodeCollection[string.Format("{0}::{1}", p.x + i, p.z + j)];
                    }

                }
            //Else, we need to add this node to the dictionary
            this.pathNodeCollection.Add(p.id, p);
            return p;
        }

        bool jwf_existsCollidable(PathFindingNode node, Int32 dist)
        {
            //return false;
            //Does a collidable object exist within the quadrent 
            //depicted by 'node' and 'dist'?
            foreach (Object3D model in stage.Collidable)
            {
                if (model.Translation.X > (node.x - dist - model.ObjectBoundingSphereRadius - 164) &&
                   model.Translation.X < (node.x + dist + model.ObjectBoundingSphereRadius + 164) &&
                   model.Translation.Z > (node.z - dist - model.ObjectBoundingSphereRadius - 164) &&
                   model.Translation.Z < (node.z + dist + model.ObjectBoundingSphereRadius + 164))
                {
                    return true;
                }
            }


            return false;
        }

        bool setUpNavigationGraph(Int32 x, Int32 z, Int32 dist)
        {
            /*Creates a navigation graph based off quad-tree graph. The purpose of this
             * graph is to traversed via A* algorithm for NPAgent treasure finding.*/

            //Initiate naviagation graph dictionary
            this.pathNodeCollection = new Dictionary<string, PathFindingNode>();

            //Set up the quad tree navigation graph
            jwf_QuadTree(new PathFindingNode(this.stage, x, z), dist);

            return true;
        }


        public override void Draw(GameTime gameTime)
        {
            Matrix[] modelTransforms = new Matrix[stage.WayPoint3D.Bones.Count];
            foreach (KeyValuePair<string, PathFindingNode> navNode in pathNodeCollection)
            {
                //Should we draw this node?
                if (!navNode.Value.enabled && !showAllNodes)
                    continue;

                // draw the Path markers
                foreach (ModelMesh mesh in stage.WayPoint3D.Meshes)
                {
                    stage.WayPoint3D.CopyAbsoluteBoneTransformsTo(modelTransforms);
                    foreach (BasicEffect effect in mesh.Effects)
                    {
                        effect.EnableDefaultLighting();
                        if (stage.Fog)
                        {
                            effect.FogColor = Color.Black.ToVector3();
                            effect.FogStart = stage.FogStart;
                            effect.FogEnd = stage.FogEnd;
                            effect.FogEnabled = true;
                        }
                        else effect.FogEnabled = false;
                        effect.DirectionalLight0.DiffuseColor = navNode.Value.nodeColor;
                        effect.AmbientLightColor = navNode.Value.nodeColor;
                        effect.DirectionalLight0.Direction = stage.LightDirection;
                        effect.DirectionalLight0.Enabled = true;
                        effect.View = stage.View;
                        effect.Projection = stage.Projection;
                        effect.World = Matrix.CreateTranslation(new Vector3(navNode.Value.x, navNode.Value.y, navNode.Value.z)) * modelTransforms[mesh.ParentBone.Index];
                    }
                    stage.setBlendingState(true);
                    mesh.Draw();
                    stage.setBlendingState(false);
                }
            }
        }
    }

}
