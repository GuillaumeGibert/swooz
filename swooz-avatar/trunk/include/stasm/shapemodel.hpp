// $shapemodel.hpp 3.0 milbo$

#if !defined(shapemodel_hpp)
#define shapemodel_hpp

#include "stasm.hpp"
#include "asmsearch.hpp"

// namespace swStasm
// {


	double xShapeExtent(const GslMat::SHAPE &Shape);
	double yShapeExtent(const GslMat::SHAPE &Shape);
	bool fPointUsed(const GslMat::SHAPE &Shape, int iPoint);
	bool fPointUsed(const double x, const double y);

	double PointToPointDist(const GslMat::SHAPE &Shape1, int iPoint1,
				const GslMat::SHAPE &Shape2, int iPoint2);

	void JitterPoints(GslMat::SHAPE &Shape);

	GslMat::StasmMat AlignShape(GslMat::SHAPE &Shape,                // io
			const GslMat::SHAPE &AnchorShape,   // in
			const GslMat::StasmVec *pWeights=NULL);  // in: can be NULL

	GslMat::StasmMat GetAlignTransform(const GslMat::SHAPE &Shape,           // io
			      const GslMat::SHAPE &AnchorShape);    // in

	GslMat::SHAPE
	ConformShapeToModel(GslMat::StasmVec &b,             // io
		const GslMat::SHAPE &Shape,             // in
		const ASM_MODEL &Model,         // in
		int iLev,                       // in
		bool fShapeModelFinalIter);     // in

	GslMat::SHAPE TransformShape(const GslMat::SHAPE &Shape, const GslMat::StasmMat &TransformMat);

	GslMat::SHAPE TransformShape(const GslMat::SHAPE &Shape,                // in
			     double x0, double y0, double z0,   // in
			     double x1, double y1, double z1);  // in

	void TransformInPlace(GslMat::SHAPE &Shape,                      // io
			      double x0, double y0, double z0,   // in
			      double x1, double y1, double z1);  // in
// };

#endif // shapemodel_hpp
