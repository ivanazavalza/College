/* Jared Fowler   yrolg4tseuq@sbcglobal.net
 * Project: P1 - Model Terrain and Treasures
 * Class: Comp565 with Prof. Barnes
 * 
 * 
 *  Changes made by Jared Fowler include the following:
 *      -Commentary added throughout code for better understanding
 *      -Some type int were changed to UInt32  
 * 
 * 
 */

/*  
    Copyright (C) 2015 G. Michael Barnes
  
    The file IndexVertexBuffers.cs is part of AGMGSKv6 a port and update of AGXNASKv5 from
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
    /// IndexVertexBuffers defines variables and properties shared
    /// by all indexed-vertex meshes.
    /// Since the vertex type can change, vertices should be defined
    /// in subclasses.
    /// </summary>
    public abstract class IndexVertexBuffers : DrawableGameComponent
    {
        //Variables------------------------------------------------------------------------
        protected Stage stage;
        protected string name;
        protected UInt32 range;
        protected UInt32 nVertices;
        protected UInt32 nIndices;
        protected VertexBuffer vb = null;
        protected IndexBuffer ib = null;
        protected UInt32[] indices;       //Note: DirectX requires clock-wise indices

        public IndexVertexBuffers(Stage theStage, string label)
            : base(theStage)
        {
            stage = theStage;
            name = label;
        }

        // Properties

        public VertexBuffer VB
        {
            get { return vb; }
            set { vb = value; }
        }

        public IndexBuffer IB
        {
            get { return ib; }
            set { ib = value; }
        }
    }
}
