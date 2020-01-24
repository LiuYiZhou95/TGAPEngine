﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace TGAPE_CS.Engine.VectorMath
{
    public static class MatrixHelper
    {
        public static Matrix4x4 ApplyLocalTranslation(Matrix4x4 matrix, Vector3 translation)
        {
            var result = matrix;
            result.M41 = matrix.M41
                    + matrix.M11 * translation.X
                    + matrix.M21 * translation.X
                    + matrix.M31 * translation.X;
            result.M42 = matrix.M42
                    + matrix.M12 * translation.Y
                    + matrix.M22 * translation.Y
                    + matrix.M32 * translation.Y;
            result.M43 = matrix.M43
                    + matrix.M13 * translation.Z
                    + matrix.M23 * translation.Z
                    + matrix.M33 * translation.Z;
            return result;
        }
        public static float[] ConvertToAnArray(Matrix4x4 matrix)
        {
            return new float[16] { matrix.M11, matrix.M12, matrix.M13, matrix.M14,
                                   matrix.M21, matrix.M22, matrix.M23, matrix.M24,
                                   matrix.M31, matrix.M32, matrix.M33, matrix.M34,
                                   matrix.M41, matrix.M42, matrix.M43, matrix.M44};
        }
        public static Matrix4x4 CreateFromAnArray(float[] array)
        {
            return new Matrix4x4(array[0], array[1], array[2], array[3],
                                 array[4], array[5], array[6], array[7],
                                 array[8], array[9], array[10], array[11],
                                 array[12], array[13], array[14], array[15]);
        }

    }
}
