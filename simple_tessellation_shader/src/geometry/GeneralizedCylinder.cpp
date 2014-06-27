#include"GeneralizedCylinder.h"
#include "Mathlib.h"

GeneralizedCylinder::GeneralizedCylinder(): mVertices(),mIndices()
{}

GeneralizedCylinder::GeneralizedCylinder(Path const& b , Contour const& c, std::vector<float> const& dicken,std::vector<float> const& water, int off): 
	mBezierPath(b), 
	mContour(c),
	mRadii(dicken),
	mOffset(off),
	mColor(water),
	_doneInitGL(false),
	mAge()
{
	int bezierpoints = mBezierPath.Points().size();
	for(int i = 0; i < bezierpoints; ++i)
	{
		
		nv::matrix4f motion = mBezierPath.Point(i).mValue;
		
		float dicke = getThickness(i);
		
		nv::matrix4f scaleMatrix;

		if(dicke > 0.00001f)
			scaleMatrix.set_scale(nv::vec3f(dicke,0.0f,dicke));
		else
			scaleMatrix.set_scale(nv::vec3f(0.1f,0.1f,0.1f));
	
		for(int j = 0; j < mContour.numberOfPoints(); ++j)
		{
			nv::vec4f temp;
			temp = motion* scaleMatrix* mContour.Bezierpoint(j);

			mAge.push_back(dicke);
			mVertices.push_back(nv::vec3f(temp));

		}
	}
	calculateColors();
	calculateIndices();
	calculateNormals();
	calculateTexCoords();
}

float GeneralizedCylinder::getThickness(int i)
{
	if(mBezierPath.Point(i).mIsSamplePoint >= 0)
	{
		return mRadii[mBezierPath.Point(i).mIsSamplePoint];
	}
	else
	{
		int smallerBorderIndex = 0;
		int higherBorderIndex = 0 ;
		float smallerBorder = 0.0f;
		//get smaller border
		for( int j = i ; j >= 0; --j)
		{
			if(mBezierPath.Point(j).mIsSamplePoint >= 0)
			{
				smallerBorder = mRadii[mBezierPath.Point(j).mIsSamplePoint];
				smallerBorderIndex = j;
				break;
			}
		}

		float higherBorder = 0.0f;
		//get higher border
		for(int j = i ; j < mBezierPath.Points().size(); ++j)
		{
			if(mBezierPath.Point(j).mIsSamplePoint >= 0)
			{
				higherBorder = mRadii[mBezierPath.Point(j).mIsSamplePoint];
				higherBorderIndex =j;
				break;
			}

		}


		float totalLength = mBezierPath.length(smallerBorderIndex,higherBorderIndex);
		float partialLength = mBezierPath.length(smallerBorderIndex,i);

		float deltaDicke = abs(smallerBorder - higherBorder);

		float neudeDicke = (partialLength/totalLength)*deltaDicke;

		if(smallerBorder < higherBorder)
		{
			return smallerBorder+neudeDicke;
		}
		else
		{
			return smallerBorder-neudeDicke;
		}
	}
}

void GeneralizedCylinder::calculateColors()
{
	std::vector<float> inputWater = mColor;
	mColor.clear();
	
	if(inputWater.size() != mRadii.size())
		std::cout<<inputWater.size()<<" "<<mRadii.size()<<std::endl;
	
	int bezierpoints = mBezierPath.Points().size();
	for(int i = 0; i < bezierpoints; ++i)
	{
		if(mBezierPath.Point(i).mIsSamplePoint >=0)
		{
			for(int k = 0; k< mContour.numberOfPoints(); ++k)
			{
				mColor.push_back(inputWater[mBezierPath.Point(i).mIsSamplePoint]);
			}
		}
		else
		{
			int smallerBorderIndex = 0;
			int higherBorderIndex = 0 ;
			float smallerBorder = 0.0f;
			//get smaller border
			for( int j = i ; j >= 0; --j)
			{
				if(mBezierPath.Point(j).mIsSamplePoint >= 0)
				{
					for(int k = 0; k< mContour.numberOfPoints(); ++k)
					{			
						mColor.push_back(inputWater[mBezierPath.Point(j).mIsSamplePoint]);
					}
					break;
				}
			}
		}
		
	}

}

void GeneralizedCylinder::calculateIndices()
{
	int contourOffset = mContour.numberOfPoints();
	int firstDown,secondDown,thirdUp;
	int firstUp,secondUp,thirdDown;

	for(int i = 0; i <mBezierPath.Points().size()-1; ++i)
	{
		for(int j = 0 ; j < mContour.numberOfPoints() ; ++j)
		{
			firstDown	= ( j		  + (i*contourOffset)) + mOffset	;
			secondDown	= ((j+1)	  + (i*contourOffset)) % contourOffset + (i*contourOffset)+ mOffset;
			thirdUp		= ((j+contourOffset) + (i*contourOffset))+ mOffset;
			
			
			mIndices.push_back(thirdUp);
			mIndices.push_back(secondDown);
			mIndices.push_back(firstDown);

			firstUp		= (j+contourOffset		+ (i*contourOffset)) + mOffset;
			secondUp	= ((j+1+contourOffset)	+ (i*contourOffset)) % contourOffset + ((i+1)*contourOffset) + mOffset;
			thirdDown	= ((j+1)				+ (i*contourOffset)) % contourOffset + (i*contourOffset)	 + mOffset;

			
			mIndices.push_back(firstUp);
			mIndices.push_back(secondUp);
			mIndices.push_back(thirdDown);

		}
	}
}

void GeneralizedCylinder::calculateNormals()
{
	std::vector<nv::vec3f> tempNormals(mVertices.size(),nv::vec3f(0.0f));
	
	for(int i = 0 ; i < mIndices.size(); i+=3)
	{
		//first face indices
		int firstIndex, secondIndex,thirdIndex;

		firstIndex = mIndices[i]-mOffset;
		secondIndex = mIndices[i+1]-mOffset;
		thirdIndex = mIndices[i+2]-mOffset;
		
	
		//points belonging to first face		
		nv::vec3f first = mVertices[firstIndex];
		nv::vec3f second= mVertices[secondIndex];
		nv::vec3f third = mVertices[thirdIndex];
		
		nv::vec3f edge1 = first-second;
		nv::vec3f edge2 = third-second;
		
		nv::vec3f normal;
		
		normal = nv::normalize(-Mathlib::cross(edge1,edge2));
							
		tempNormals[firstIndex]+=normal;
		tempNormals[secondIndex]+=normal;
		tempNormals[thirdIndex]+=normal;


		if(nv::length(edge1-edge2)< 0.0001 || edge1.x != edge1.x || edge2.x!=edge2.x)
		{
			std::cout<<"FIRST: "<<firstIndex<<" "<<first<<std::endl;
			std::cout<<"SECOND: "<<secondIndex<<" "<<second<<std::endl;
			std::cout<<"THIRD: "<<thirdIndex<<" "<<third<<std::endl;
			std::cout<<"EDGE1: "<<edge1.x<<" "<<edge1.y<<" "<<edge1.z<<"\n";
			std::cout<<"EDGE2: "<<edge2.x<<" "<<edge2.y<<" "<<edge2.z<<"\n";
			std::cout<<"NORMAL: "<<normal.x<<" "<<normal.y<<" "<<normal.z<<"\n\n";
		}
	}
	
	for (int k = 0; k < tempNormals.size(); ++k)
	{
		mNormals.push_back(nv::normalize(tempNormals[k]));
	}
}

void GeneralizedCylinder::calculateTexCoords() {
	for(unsigned int i = 0; i < (mVertices.size()/36); ++i)
	{//2*24
		mTexCoords.push_back(nv::vec2f(0.98f, 0.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 0.98f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 2.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 3.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 4.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 5.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 6.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 7.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 8.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 9.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 10.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 10.98f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 12.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 10.98f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 10.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 9.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 8.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 7.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 6.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 5.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 4.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 3.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 2.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.98f, 0.98f/12.0f));

		mTexCoords.push_back(nv::vec2f(0.0f, 0.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 1.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 2.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 3.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 4.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 5.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 6.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 7.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 8.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 9.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 10.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 11.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 12.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 11.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 10.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 9.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 8.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 7.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 6.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 5.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 4.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 3.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 2.0f/12.0f));
		mTexCoords.push_back(nv::vec2f(0.0f, 1.0f/12.0f));
	}
}

void GeneralizedCylinder::initGL(){
	std::vector<nv::vec3f>    const& vertex		= mVertices;
	std::vector<unsigned int> const& index		= mIndices;
	std::vector<nv::vec3f>    const& normal		= mNormals;
	std::vector<nv::vec2f>    const& texCoord	= mTexCoords;
	std::vector<float>    const& age			= mAge;
	
	//create VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	//VBO positions
	glGenBuffers(1, &_VBOpositions);
	glBindBuffer(GL_ARRAY_BUFFER, _VBOpositions);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(nv::vec3f), vertex.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
	);

	//VBO normals
	glGenBuffers(1, &_VBOnormals);
	glBindBuffer(GL_ARRAY_BUFFER, _VBOnormals);
	glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(nv::vec3f), normal.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//VBO texCoords
	glGenBuffers(1, &_VBOtexCoords);
	glBindBuffer(GL_ARRAY_BUFFER, _VBOtexCoords);
	glBufferData(GL_ARRAY_BUFFER, texCoord.size() * sizeof(nv::vec2f), texCoord.data(), GL_STATIC_DRAW);
		
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//VBO age
	glGenBuffers(1, &_VBOage);
	glBindBuffer(GL_ARRAY_BUFFER, _VBOage);
	glBufferData(GL_ARRAY_BUFFER, age.size() * sizeof(float), age.data(), GL_STATIC_DRAW);
		
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//IBO
	glGenBuffers(1, &_VBOindices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBOindices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(unsigned int), index.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	_indexCount = index.size();

	GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

	_doneInitGL = true;	
}

//Getter
std::vector<nv::vec3f>* GeneralizedCylinder::vertices()
{
	return &mVertices;
}

std::vector<nv::vec3f>* GeneralizedCylinder::normals()
{
	return &mNormals;
}

std::vector<unsigned int>* GeneralizedCylinder::indices()
{
	return &mIndices;
}

std::vector<float>* GeneralizedCylinder::color()
{
	return &mColor;
}

GLuint GeneralizedCylinder::getVAO() {
	if(!_doneInitGL)
		initGL();
	return _vao;
}

GLsizei GeneralizedCylinder::getIndexCount() const {
	return _indexCount;
}