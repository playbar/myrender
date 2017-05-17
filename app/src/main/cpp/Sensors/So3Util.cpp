//
// Created by houguoli on 2017/5/17.
//

#include "So3Util.h"

double So3Util::ONE_6TH = 0.1666666716337204;
double So3Util::ONE_20TH = 0.1666666716337204;
Vector3dJ So3Util::temp31;
Vector3dJ So3Util::sO3FromTwoVecN;
Vector3dJ So3Util::sO3FromTwoVecA;
Vector3dJ So3Util::sO3FromTwoVecB;
Vector3dJ So3Util::sO3FromTwoVecRotationAxis;
Matrix3x3d So3Util::sO3FromTwoVec33R1;
Matrix3x3d So3Util::sO3FromTwoVec33R2;
Vector3dJ So3Util::muFromSO3R2;
Vector3dJ So3Util::rotationPiAboutAxisTemp;