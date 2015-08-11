/* Jared Fowler   yrolg4tseuq@sbcglobal.net
 * Project: P1 - Model Terrain and Treasures
 * Class: Comp565 with Prof. Barnes
 * 
 * 
 *  Changes made by Jared Fowler include the following:
 *      -Commentary added throughout code for better understanding
 *      -Line and variable spacing more orgainzed
 *      -Removed second constructor which only differed in scaling capability. 
 *       Users will now be forced to input a scaling vector3.
 *      -Constructor now has input parameters for step, stepsize, and bounding
 *       radius.
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */


/*  
    Copyright (C) 2015 G. Michael Barnes
 
    The file Object3D.cs is part of AGMGSKv6 a port and update of AGXNASKv5 from
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

namespace AGMGSKv6 {

   /// <summary>
   /// Defines location and orientation.
   /// Object's orientation is a 4 by 4 XNA Matrix. 
   /// Object's location is Vector3 describing it position in the stage.
   /// Has good examples of C# Properties (Location, Orientation, Right, Up, and At).
   /// These properties show how the 4 by 4 XNA Matrix values are
   /// stored and what they represent.
   /// Properties Right, Up, and At get and set values in matrix orientation.
   /// Right, the object's local X axis, is the lateral unit vector.
   /// Up, the object's local Y axis, is the vertical unit vector.
   /// At, the object's local Z axis, is the forward unit vector.
   /// 
   /// 2/1/2015  last changed
   /// </summary>


    public class Object3D
    {
        //Variables----------------------------------------------------------------------------
        private Model3D model;
        private string  name;                                  // string identifier
        private Stage   stage;                                 // framework stage object
        private Matrix  orientation;                           // object's orientation 
        private Vector3 scales;                                // object's scale factors
        private float   pitch, roll, yaw;                      // changes in rotation
        private Int16  step, stepSize;                        // values for stepping
        private Vector3 objectBoundingSphereCenter;            // bounding sphere information
        private float   objectBoundingSphereRadius;
        private Matrix  objectBoundingSphereWorld;

        //Constructors --------------------------------------------------------------------------

        /// <summary>
        /// Object that places and orients itself.
        /// </summary>
        /// <param name="theStage">   the stage containing object </param> 
        /// <param name="aModel">     how the object looks        </param> 
        /// <param name="label">      name of object              </param> 
        /// <param name="position">   position in stage           </param> 
        /// <param name="orientAxis"> axis to orient on           </param> 
        /// <param name="radians">    orientation rotation        </param> 
        public Object3D(Stage stage, Model3D model, string name, Vector3 position,
                        Vector3 orientAxis, float radiansRot, Vector3 scale, 
                        Int16 step = 1, Int16 stepSize = 10, float boundingRadius = 0.0f)
        {
            this.scales   = scale;
            this.stage    = stage;
            this.model    = model;
            this.name     = name;
            this.step     = step;
            this.stepSize = stepSize;

            this.objectBoundingSphereRadius = boundingRadius;

            //Initialize rotating factors
            pitch = yaw = roll = 0.0f;

            //Configure resulting orientation
            orientation  = Matrix.Identity;
            orientation *= Matrix.CreateScale(scales);
            orientation *= Matrix.CreateFromAxisAngle(orientAxis, radiansRot);
            orientation *= Matrix.CreateTranslation(position);

            scaleObjectBoundingSphere();
        }

        //Properties-----------------------------------------------------------------------------
        public string Name
        {
            get { return name; }
            set { name = value; }
        }

        public Matrix ObjectBoundingSphereWorld
        {
            get { return objectBoundingSphereWorld; }
        }

        public float ObjectBoundingSphereRadius
        {
            get { return objectBoundingSphereRadius; }
        }

        public Matrix Orientation
        {
            get { return orientation; }
            set { orientation = value; }
        }

        public Vector3 Translation
        {
            get { return orientation.Translation; }
            set { orientation.Translation = value; }
        }

        public Vector3 Up
        {
            get { return orientation.Up; }
            set { orientation.Up = value; }
        }

        public Vector3 Down
        {
            get { return orientation.Down; }
            set { orientation.Down = value; }
        }

        public Vector3 Right
        {
            get { return orientation.Right; }
            set { orientation.Right = value; }
        }

        public Vector3 Left
        {
            get { return orientation.Left; }
            set { orientation.Left = value; }
        }

        public Vector3 Forward
        {
            get { return orientation.Forward; }
            set { orientation.Forward = value; }
        }

        public Vector3 Backward
        {
            get { return orientation.Backward; }
            set { orientation.Backward = value; }
        }

        public float Pitch
        {
            get { return pitch; }
            set { pitch = value; }
        }

        public float Yaw
        {
            get { return yaw; }
            set { yaw = value; }
        }

        public float Roll
        {
            get { return roll; }
            set { roll = value; }
        }

        public Int16 Step
        {
            get { return step; }
            set { step = value; }
        }

        public Int16 StepSize
        {
            get { return stepSize; }
            set { stepSize = value; }
        }

        public void reset()
        {
            pitch = roll = yaw = 0;
            step = 0;
        }

        //Methods-------------------------------------------------------------------------------

        /// <summary>
        ///  Does the Object3D's new position collide with any Collidable Object3Ds ?
        /// </summary>
        /// <param name="position"> position Object3D wants to move to </param>
        /// <returns> true when there is a collision </returns>
        private bool collision(Vector3 position)
        {
            //Local Variables
            float distance;
            Vector2 pos2D;
            Vector2 obj2D;
            
            //Initialization
            pos2D = new Vector2(position.X, position.Z);

            //Check for collision between this object and any other in the stage
            foreach (Object3D obj3d in stage.Collidable)
            {
                //Get the vector position of the next stage collidable object 'obj'
                obj2D = new Vector2(obj3d.Translation.X, obj3d.Translation.Z);

                //Calculate the distance between this object and 'obj'
                distance = Vector2.Distance(pos2D, obj2D);

                //Make sure the comparing object isn't itself, and if distance is less than spere radiuses, there is a collision
                if ( (!this.Equals(obj3d)) && (distance <= ObjectBoundingSphereRadius + obj3d.ObjectBoundingSphereRadius) )
                {
                    //Print out to stage's info
                    stage.setInfo(18,
                        string.Format("Object3D.collison(): {0} BR = {1,5:f2} with {2} BR = {3,5:f2}, {4,5:f2} >= {5,5:f2} distance",
                        Name, ObjectBoundingSphereRadius, obj3d.Name, obj3d.ObjectBoundingSphereRadius,
                        ObjectBoundingSphereRadius + obj3d.ObjectBoundingSphereRadius, distance));

                    return true;
                }
            }

            //No collisions!
            return false;
        }

        //-----------------------------------------------------------------------------------------
        private void scaleObjectBoundingSphere()
        {
            /*This is a very simplified function. We will only concern ourselves with x,z 
             * scaling options. The largest of the two will be what will scale the bounds 
             * of our objects. This is done in order to avoid creating large circular spheres 
             * for objects such as trees which have an extremely large y, but small x,z */

            if (scales.X >= scales.Z)
                objectBoundingSphereRadius = this.model.BoundingSphereRadius * scales.X;

            else 
                objectBoundingSphereRadius = model.BoundingSphereRadius * scales.Z;

        }

        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Update the object's orientation matrix so that it is rotated to 
        /// look at target. AGXNASK is terrain based -- so all turn are wrt flat XZ plane.
        /// AGXNASK assumes models are made to "look" -Z 
        ///    toObj is a vector to this Object3D
        ///    toTarget is a vector to the target (nextGoal on path)
        ///    axis is the vector to rotate about (positive Y)
        /// </summary>
        /// <param name="target"> to look at</param>
        public void turnToFace(Vector3 target)
        {
            //Local Variables
            Vector3 axis, toTarget, toObj;
            double radian, aCosDot;

            //Get locations of this object and target object on XZ plane
            toObj  = new Vector3(Translation.X, 0, Translation.Z);
            target = new Vector3(target.X, 0, target.Z);

            //Get vector to target and normalize this vector
            toTarget = target - toObj;
            toTarget.Normalize();

            // if vectors are identical (w/in epsilon 0.05) return, no need to turnToFace
            if (Vector3.Distance(toTarget, Forward) <= 0.05) 
                return;

            // if vectors are reversed (w/in epsilon 0.05) nudgle alittle
            if (Vector3.Distance(Vector3.Negate(toTarget), Forward) <= 0.05)
            {
                toTarget.X += 0.05f;
                toTarget.Z += 0.05f;
                toTarget.Normalize();
            }

            //Get axis of rotation. Remember right hand rule...order maters
            axis = Vector3.Cross(toTarget, Forward);
            axis.Normalize();

            //Get the cons(angle) of rotation
            aCosDot = Math.Acos(Vector3.Dot(toTarget, Forward));

            //Test and adjust direction of rotation into radians
            if (aCosDot == 0) 
                radian = Math.PI * 2;
            else if (aCosDot == Math.PI) 
                radian = Math.PI;
            else if (axis.X + axis.Y + axis.Z < 0) 
                radian = (float)(2 * Math.PI - aCosDot);
            else 
                radian = -aCosDot;

            if (Double.IsNaN(radian))
            {  
                // validity check, this should not happen
                stage.setInfo(19, "error:  Object3D.turnToFace() radian is NaN");
                return;
            }
            else
            {  
                //Move object back to origin
                Vector3 objectLocation = Translation;
                orientation *= Matrix.CreateTranslation(-1 * objectLocation);

                //Rotate object
                orientation *= Matrix.CreateFromAxisAngle(axis, (float)radian);

                //Fix orientation to have y axis as up direction
                orientation.Up = Vector3.Up;  // correct for flipped from negative axis of rotation

                //Move object back to where it was before
                orientation *= Matrix.CreateTranslation(objectLocation);
            }
        }

        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// The location is first saved and the model is translated
        /// to the origin before any rotations are applied.  Objects rotate about their
        /// center.  After rotations, the location is reset and updated iff it is not
        /// outside the range of the stage (stage.withinRange(String name, Vector3 location)).  
        /// When movement would exceed the stage's boundaries the model is not moved 
        /// and a message is displayed.
        /// </summary>
        public void updateMovableObject()
        {
            //Local Variables
            Vector3 startLocation;
            Vector3 stopLocation;

            //Begin by setting both start and stop to the models current location on the stage
            startLocation = stopLocation = Translation;

            //Update the objects orientation.

            //Move object back to origin
            Orientation *= Matrix.CreateTranslation(-1 * Translation);

            //Rotate the object via yaw, pitch, and roll
            Orientation *= Matrix.CreateRotationY(yaw);
            Orientation *= Matrix.CreateRotationX(pitch);
            Orientation *= Matrix.CreateRotationZ(roll);

            //Move the object "forward" which is based off objects orientation
            stopLocation += ((step * stepSize) * Forward);
   
            //Have we gone off the terrain or collided with another object?
            if (stage.withinRange(this.Name, stopLocation)){ //Are we still on the map?

                //Have we collided with another object on the map?
                if (model.IsCollidable && collision(stopLocation)){
                    //If so, move the model back to where it was before
                    Orientation *= Matrix.CreateTranslation(startLocation);
                } 
                else //No collision and on suface move
                    Orientation *= Matrix.CreateTranslation(stopLocation);  // move forward
            }
                
            else //Out of bounds... restore original position
                Orientation *= Matrix.CreateTranslation(startLocation);  
  
        }

        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Update the scale matrix for the object's bounding sphere
        /// </summary>
        /// <param name="modelRadius"></param>
        public void updateBoundingSphere()
        {
            /* This function takes care of moving and scaling the object which is represented
             * by a bounding sphere. We need to scale and translate the bounding sphere object
             * to this objects location */

            objectBoundingSphereCenter = Translation;

            //Scale the bounding sphere object
            objectBoundingSphereWorld  = Matrix.CreateScale(objectBoundingSphereRadius * 2);

            //Translate the sphere out to the objects location
            objectBoundingSphereWorld *= Matrix.CreateTranslation(objectBoundingSphereCenter);
        }

    }

}