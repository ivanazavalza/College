/* Jared Fowler   yrolg4tseuq@sbcglobal.net
 * Project: P1 - Model Terrain and Treasures
 * Class: Comp565 with Prof. Barnes
 * 
 * 
 *  Changes made by Jared Fowler include the following:
 *      -Commentary added throughout code for better understanding
 *      -Integrated and removed second addObject function
 *      -isCollidable now set in constructor with default argument
 *      -Changed fog color to black 
 * 
 * 
 * 
 * 
 * 
 * 
 */

/*  
    Copyright (C) 2015 G. Michael Barnes
 
    The file Model3D.cs is part of AGMGSKv6 a port and update of AGXNASKv5 from
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
    /// Stationary modeled object in the stage.  The SK565 Stage class creates several examples:
    /// the temple and the four dogs.
    /// 
    /// Models compute a bounding cylinder (x, z) for collision tests.
    /// 
    /// 12/31/2014 last changed
    /// </summary>
    public class Model3D : DrawableGameComponent
    {

        //Variables------------------------------------------------------------------------------------
        protected string name;
        protected Stage  stage;
        protected Model  model = null;
        protected bool   isCollidable;

        // Model3D's mesh BoundingSphere values
        protected Vector3 boundingSphereCenter;
        protected float   boundingSphereRadius = 0.0f;
        protected Matrix  boundingSphereWorld;

        // Model3D's object instance collection 
        protected List<Object3D> instance;

        //Constructors-----------------------------------------------------------------------------------
        public Model3D(Stage stage, string name, string fileOfModel, bool isCollidable = false)
            : base(stage)
        {
            this.name = name;
            this.stage = stage;
            this.isCollidable = isCollidable;

            instance = new List<Object3D>();
            model = stage.Content.Load<Model>(fileOfModel);

            // compute the translation to the model's bounding sphere
            // center and radius;
            float minX, minY, minZ, maxX, maxY, maxZ;
            minX = minY = minZ = Int32.MaxValue;
            maxX = maxY = maxZ = Int32.MinValue;

            for (int i = 0; i < model.Meshes.Count; i++)
            {
                // See if this mesh extends the bounding sphere.
                BoundingSphere aBoundingSphere = model.Meshes[i].BoundingSphere;
                boundingSphereRadius = aBoundingSphere.Radius;

                // minimum value
                if ((aBoundingSphere.Center.X - aBoundingSphere.Radius) < minX)
                    minX = aBoundingSphere.Center.X - aBoundingSphere.Radius;
                if ((aBoundingSphere.Center.Y - aBoundingSphere.Radius) < minY)
                    minY = aBoundingSphere.Center.Y - aBoundingSphere.Radius;
                if ((aBoundingSphere.Center.Z - aBoundingSphere.Radius) < minZ)
                    minZ = aBoundingSphere.Center.Z - aBoundingSphere.Radius;

                // maximum value
                if ((aBoundingSphere.Center.X + aBoundingSphere.Radius) > maxX)
                    maxX = aBoundingSphere.Center.X + aBoundingSphere.Radius;
                if ((aBoundingSphere.Center.Y + aBoundingSphere.Radius) > maxY)
                    maxY = aBoundingSphere.Center.Y + aBoundingSphere.Radius;
                if ((aBoundingSphere.Center.Z + aBoundingSphere.Radius) > maxZ)
                    maxZ = aBoundingSphere.Center.Z + aBoundingSphere.Radius;
            }

            // get the diameter of model's bounding sphere
            // radius temporarily holds the largest diameter
            if ((maxX - minX) > boundingSphereRadius) boundingSphereRadius = maxX - minX;

            // Since a bounding cylinder is used for collision height values not needed
            // if ((maxY - minY) > boundingSphereRadius) boundingSphereRadius = maxY - minY;
            if ((maxZ - minZ) > boundingSphereRadius) boundingSphereRadius = maxZ - minZ;

            // set boundingSphereRadius
            boundingSphereRadius = boundingSphereRadius * 1.05f / 2.0f;  // set the radius from largest diameter  

            // set the center of model's bounding sphere
            boundingSphereCenter = new Vector3(minX + boundingSphereRadius, minY + boundingSphereRadius, minZ + boundingSphereRadius);
        }

        //Methods----------------------------------------------------------------------------------------
        public Vector3 BoundingSphereCenter
        {
            get { return boundingSphereCenter; }
        }

        public float BoundingSphereRadius
        {
            get { return boundingSphereRadius; }
        }

        public List<Object3D> Instance
        {
            get { return instance; }
        }

        public bool IsCollidable
        {
            get { return isCollidable; }
            set { isCollidable = value; }
        }

        //-----------------------------------------------------------------------------------------------
        /// <summary>
        /// Create a new Object3D, place it on the stage, add to Model3D's instance collection, 
        /// and if collidable add to collision collection.
        /// </summary>
        /// <param name="position"> location of new Object3D</param>
        /// <param name="orientAxis"> axis of rotation</param>
        /// <param name="radians"> rotation on orientAxis</param>
        /// <param name="scales"> size the Object3D</param>
        /// <returns> the new Object3D</returns>
        public Object3D addObject(Vector3 position, Vector3 orientAxis, float radians, Vector3 scales)
        {
            Object3D obj3d = new Object3D(stage, this, String.Format("{0}.{1}", name, instance.Count),
                                          position, orientAxis, radians, scales);

            //This update is needed for objects such as walls, trees, etc.. objects who have Model3D as parent
            obj3d.updateBoundingSphere();

            //Add an instance of this object to the list
            instance.Add(obj3d);

            //If the object is collidable, also add it to the stages collidable list
            if (IsCollidable) 
                stage.Collidable.Add(obj3d);

            return obj3d;
        }

        //-----------------------------------------------------------------------------------------------
        public override void Draw(GameTime gameTime)
        {
            Matrix[] modelTransforms = new Matrix[model.Bones.Count];

            foreach (Object3D obj3d in instance)
            {
                foreach (ModelMesh mesh in model.Meshes)
                {
                    model.CopyAbsoluteBoneTransformsTo(modelTransforms);

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
                        effect.DirectionalLight0.DiffuseColor = stage.DiffuseLight;
                        effect.AmbientLightColor = stage.AmbientLight;
                        effect.DirectionalLight0.Direction = stage.LightDirection;
                        effect.DirectionalLight0.Enabled = true;
                        effect.View = stage.View;
                        effect.Projection = stage.Projection;
                        effect.World = modelTransforms[mesh.ParentBone.Index] * obj3d.Orientation;
                    }
                    mesh.Draw();
                }

                // draw the bounding sphere with blending ?
                if (stage.DrawBoundingSpheres && IsCollidable)
                {
                    foreach (ModelMesh mesh in stage.BoundingSphere3D.Meshes)
                    {
                        model.CopyAbsoluteBoneTransformsTo(modelTransforms);

                        foreach (BasicEffect effect in mesh.Effects)
                        {
                            effect.EnableDefaultLighting();

                            if (stage.Fog)
                            {
                                effect.FogColor = Color.Black.ToVector3();
                                effect.FogStart = 50;
                                effect.FogEnd = 500;
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
                            effect.World = obj3d.ObjectBoundingSphereWorld * modelTransforms[mesh.ParentBone.Index];
                        }

                        stage.setBlendingState(true);
                        mesh.Draw();
                        stage.setBlendingState(false);
                    }
                }
            }
        }
    }

}