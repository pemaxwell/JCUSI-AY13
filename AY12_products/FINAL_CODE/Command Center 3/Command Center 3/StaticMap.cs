using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Configuration;



namespace command_UI
{
    class StaticMap
    {
        /// <summary>
        /// Renders an image for display
        /// </summary>
        /// <returns></returns>
        /// <remarks>Primarily this just creates an ImageURL string</remarks>
        public string Render()
        {
            string qs = "http://maps.google.com/staticmap?center={0},{1}&zoom={2}&size={3}x{4}&maptype={5}";
            string mkqs = "";

            qs = string.Format(qs, LatCenter, LngCenter, Zoom, Width, Height, Type.ToString().ToLower());


            // add markers
            foreach (var marker in _markers)
            {

                mkqs += string.Format("{0},{1},{2}|",
                    marker.Lat,
                    marker.Lng,
                    GetMarkerParams(marker.Size, marker.Color, marker.Character));
            }

            if (mkqs.Length > 0)
            {
                qs += "&markers=" + mkqs.Substring(0, (mkqs.Length - 1));
            }



            return qs;
        }

        /// <summary>
        /// Build the correct string for marker parameters
        /// </summary>
        /// <returns></returns>
        /// <remarks>
        /// (Some marker sizes such as 'tiny' won't accept character values, 
        ///     this function makes sure they'll be rendered even if the inputed parameters are wrong
        /// </remarks>
        private static string GetMarkerParams(mSize size, mColor color, string character)
        {
            string marker;

            // check if can render character
            if ((size == mSize.Normal) || (size == mSize.Mid))
            {
                if (size == mSize.Normal)
                {
                    marker = color.ToString().ToLower() + character;
                }
                else
                {
                    marker = size.ToString().ToLower() + color.ToString().ToLower() + character;
                }
            }
            else
            {
                // just render size and color - character not supported
                marker = size.ToString().ToLower() + color.ToString().ToLower();
            }

            return marker;

        }

        /// <summary>
        /// Defines a single map point to be added to a map
        /// </summary>
        public class Marker
        {

            private string _char = "";

            /// <summary>
            /// Optional single letter character
            /// </summary>
            public string Character
            {
                get { return _char; }
                set { _char = value; }
            }

            #region Auto-Properties

            public Double Lat { get; set; }
            public Double Lng { get; set; }
            public StaticMap.mSize Size { get; set; }
            public StaticMap.mColor Color { get; set; }

            #endregion

        }

        /// <summary>
        /// All Map rendering properties as enums
        /// </summary>
        #region Marker enums

        public enum mFormat
        {
            gif = 0,
            jpg = 1,
            png = 2
        }

        public enum mSize
        {
            Normal = 0,
            Mid = 1,
            Small = 2,
            Tiny = 3
        }

        public enum mColor
        {
            Black = 0,
            Brown = 1,
            Green = 2,
            Purple = 3,
            Yellow = 4,
            Blue = 5,
            Gray = 6,
            Orange = 7,
            Red = 8,
            White = 9
        }

        public enum mType
        {
            Roadmap = 0,
            Mobile = 1,
            Terrain = 3,
            Hybrid = 4
        }

        #endregion

        /// <summary>
        /// StaticMap props
        /// </summary>
        #region Properties

        private List<Marker> _markers = new List<Marker>();
        private StaticMap.mType _type = StaticMap.mType.Roadmap;

        /// <summary>
        /// List of all markers to be displayed on the map
        /// </summary>
        public List<Marker> Markers
        {
            get { return _markers; }
            set { _markers = value; }
        }

        public StaticMap.mType Type
        {
            get { return _type; }
            set { _type = value; }
        }

        /// <summary>
        /// Google maps API key - required!
        /// </summary>
        

        #region Auto-Properties

        public Double LatCenter { get; set; }
        public Double LngCenter { get; set; }
        public int Zoom { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }

        #endregion

        #endregion

    }


}

