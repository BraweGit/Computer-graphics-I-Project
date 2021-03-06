#include "stdafx.h"

void rtc_error_function( const RTCError code, const char * str )
{
	printf( "ERROR in Embree: %s\n", str );
	exit( 1 );
}

RTCError check_rtc_or_die( RTCDevice & device )
{
	const RTCError error = rtcDeviceGetError( device );

	if ( error != RTC_NO_ERROR )
	{
		printf( "ERROR in Embree: " );

		switch ( error )
		{
		case RTC_UNKNOWN_ERROR:
			printf( "An unknown error has occurred." );
			break;

		case RTC_INVALID_ARGUMENT:
			printf( "An invalid argument was specified." );
			break;

		case RTC_INVALID_OPERATION:
			printf( "The operation is not allowed for the specified object." );
			break;

		case RTC_OUT_OF_MEMORY:
			printf( "There is not enough memory left to complete the operation." );
			break;

		case RTC_UNSUPPORTED_CPU:
			printf( "The CPU is not supported as it does not support SSE2." );
			break;

		case RTC_CANCELLED:
			printf( "The operation got cancelled by an Memory Monitor Callback or Progress Monitor Callback function." );
			break;
		}

		fflush( stdout );
		exit( 1 );
	}

	return error;
}

// struktury pro ukládání dat pro Embree
namespace embree_structs
{
	struct Vertex { float x, y, z, a; };
	typedef Vertex Normal;
	struct Triangle { int v0, v1, v2; };
};

void filter_intersection( void * user_ptr, Ray & ray )
{
	/*  All hit information inside the ray is valid.
		The filter function can reject a hit by setting the geomID member of the ray to
	    RTC_INVALID_GEOMETRY_ID, otherwise the hit is accepted.The filter function is not
		allowed to modify the ray input data (org, dir, tnear, tfar), but can modify
		the hit data of the ray( u, v, Ng, geomID, primID ). */

	Surface * surface = reinterpret_cast<Surface *>( user_ptr );	
	printf( "intersection of: %s, ", surface->get_name().c_str() );
	const Vector3 p = ray.eval( ray.tfar );
	printf( "at: %0.3f (%0.3f, %0.3f, %0.3f)\n", ray.tfar, p.x, p.y, p.z );
	
	ray.geomID = RTC_INVALID_GEOMETRY_ID; // reject hit
}

void filter_occlusion( void * user_ptr, Ray & ray )
{
	/*  All hit information inside the ray is valid.
	The filter function can reject a hit by setting the geomID member of the ray to
	RTC_INVALID_GEOMETRY_ID, otherwise the hit is accepted.The filter function is not
	allowed to modify the ray input data (org, dir, tnear, tfar), but can modify
	the hit data of the ray( u, v, Ng, geomID, primID ). */

	Surface * surface = reinterpret_cast<Surface *>( user_ptr );	
	printf( "occlusion of: %s, ", surface->get_name().c_str() );
	const Vector3 p = ray.eval( ray.tfar );
	printf( "at: %0.3f (%0.3f, %0.3f, %0.3f)\n", ray.tfar, p.x, p.y, p.z );

	ray.geomID = RTC_INVALID_GEOMETRY_ID; // reject hit
}

int test( RTCScene & scene, std::vector<Surface *> & surfaces )
{
	// --- test rtcIntersect -----
	//Ray rtc_ray = Ray( Vector3( -1.0f, 0.1f, 0.2f ), Vector3( 2.0f, 0.0f, 0.0f ), 0.0f );	
	Ray rtc_ray = Ray(Vector3(45.8f, 20.5f, 1.0f), Vector3(0.0f, 0.0f, -1.0f));
	//Ray rtc_ray = Ray( Vector3( 4.0f, 0.1f, 0.2f ), Vector3( -1.0f, 0.0f, 0.0f ) );
	//rtc_ray.tnear = 0.6f;
	//rtc_ray.tfar = 2.0f;
	rtcIntersect( scene, rtc_ray ); // find the closest hit of a ray segment with the scene
	// pri filtrovani funkce rtcIntersect jsou pruseciky prochazeny od nejblizsiho k nejvzdalenejsimu
	// u funkce rtcOccluded jsou nalezene pruseciky vraceny v libovolnem poradi

	if ( rtc_ray.geomID != RTC_INVALID_GEOMETRY_ID )
	{
		Surface * surface = surfaces[rtc_ray.geomID];
		Triangle & triangle = surface->get_triangle( rtc_ray.primID );
		//Triangle * triangle2 = &( surface->get_triangles()[rtc_ray.primID] );

		// získání souřadnic průsečíku, normál, texturovacích souřadnic atd.
		const Vector3 p = rtc_ray.eval( rtc_ray.tfar );
		Vector3 geometry_normal = -Vector3( rtc_ray.Ng ); // Ng je nenormalizovaná normála zasaženého trojúhelníka vypočtená nesouhlasně s pravidlem pravé ruky o závitu
		geometry_normal.Normalize(); // normála zasaženého trojúhelníka vypočtená souhlasně s pravidlem pravé ruky o závitu
		const Vector3 normal = triangle.normal( rtc_ray.u, rtc_ray.v );
		const Vector2 texture_coord = triangle.texture_coord( rtc_ray.u, rtc_ray.v );

		printf("");
	}

	// --- test rtcOccluded -----
	rtc_ray = Ray( Vector3( -1.0f, 0.1f, 0.2f ), Vector3( 1.0f, 0.0f, 0.0f ) );
	//rtc_ray.tfar = 1.5;	
	rtcOccluded( scene, rtc_ray ); // determining if any hit between a ray segment and the scene exists
	// po volání rtcOccluded je nastavena pouze hodnota geomID, ostatni jsou nezměněny
	if ( rtc_ray.geomID == 0 )
	{
		// neco jsme nekde na zadaném intervalu (tnear, tfar) trefili, ale nevime co ani kde
	}

	return 0;
}

cv::Mat GenericRaytraceWithEmbree(RTCScene & scene, Camera & camera, std::vector<Surface *> & surfaces)
{
	printf("Start ray tracing...\n");

	cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
	cv::moveWindow("Image", 0, 0);
	

	// obrázek tří barevných kanálů, jeden pixel tvoří 3 subpixely; Blue, Green, Red
	cv::Mat image = cv::Mat(
		cv::Size(camera.width(), camera.height()), CV_32FC3);
	cv::resizeWindow("Image", image.cols, image.rows);
	

	for (int y = 0; y < image.rows; ++y)
	{
		for (int x = 0; x < image.cols; ++x)
		{
			Ray rtc_ray = camera.GenerateRay(x, y);
			rtcIntersect(scene, rtc_ray);

			// TODO: METHOD
			//Surface *surface = surfaces.at(rtc_ray.geomID);
			//Triangle triangle = surface->get_triangle(rtc_ray.primID);

			//Vector3 normal = triangle.normal(rtc_ray.u, rtc_ray.v);
			//normal.Normalize();

			// Get normal
			// Normal to RGB
			// (n+(1,1,1))/2

			if (rtc_ray.geomID != RTC_INVALID_GEOMETRY_ID)
			{
				image.at<cv::Vec3f>(int(y), int(x)) = cv::Vec3f(1, 1, 1);
			}
		}
	}
	
	cv::imshow("Image", image);
	cvWaitKey(0);
	return image;
}


/*
Seznam úkolů:

1, Doplnit TODO v souboru tracing.cpp.
*/

int main( int argc, char * argv[] )
{
	printf( "PG1, (c)2011-2016 Tomas Fabian\n\n" );	

	_MM_SET_FLUSH_ZERO_MODE( _MM_FLUSH_ZERO_ON ); // Flush to Zero, Denormals are Zero mode of the MXCSR
	_MM_SET_DENORMALS_ZERO_MODE( _MM_DENORMALS_ZERO_ON );
	RTCDevice device = rtcNewDevice( NULL ); // musíme vytvořit alespoň jedno Embree zařízení		
	check_rtc_or_die( device ); // ověření úspěšného vytvoření Embree zařízení
	rtcDeviceSetErrorFunction( device, rtc_error_function ); // registrace call-back funkce pro zachytávání chyb v Embree	

	std::vector<Surface *> surfaces;
	std::vector<Material *> materials;

	// načtení geometrie
	// 6887_allied_avenger
	//geosphere
	if ( LoadOBJ( "../../data/geosphere.obj", Vector3( 0.5f, 0.5f, 0.5f ), surfaces, materials ) < 0 )
	{
		return -1;
	}
	
	// vytvoření scény v rámci Embree
	RTCScene scene = rtcDeviceNewScene( device, RTC_SCENE_STATIC | RTC_SCENE_HIGH_QUALITY, RTC_INTERSECT1/* | RTC_INTERPOLATE*/ );
	// RTC_INTERSECT1 = enables the rtcIntersect and rtcOccluded functions

	// nakopírování všech modelů do bufferů Embree
	for ( std::vector<Surface *>::const_iterator iter = surfaces.begin();
		iter != surfaces.end(); ++iter )
	{
		Surface * surface = *iter;
		unsigned geom_id = rtcNewTriangleMesh( scene, RTC_GEOMETRY_STATIC,
			surface->no_triangles(), surface->no_vertices() );

		//rtcSetUserData, rtcSetBoundsFunction, rtcSetIntersectFunction, rtcSetOccludedFunction,
		rtcSetUserData( scene, geom_id, surface );
		//rtcSetOcclusionFilterFunction, rtcSetIntersectionFilterFunction		
		//rtcSetOcclusionFilterFunction( scene, geom_id, reinterpret_cast< RTCFilterFunc >( &filter_occlusion ) );
		//rtcSetIntersectionFilterFunction( scene, geom_id, reinterpret_cast< RTCFilterFunc >( &filter_intersection ) );

		// kopírování samotných vertexů trojúhelníků
		embree_structs::Vertex * vertices = static_cast< embree_structs::Vertex * >(
			rtcMapBuffer( scene, geom_id, RTC_VERTEX_BUFFER ) );

		for ( int t = 0; t < surface->no_triangles(); ++t )
		{
			for ( int v = 0; v < 3; ++v )
			{
				embree_structs::Vertex & vertex = vertices[t * 3 + v];

				vertex.x = surface->get_triangles()[t].vertex( v ).position.x;
				vertex.y = surface->get_triangles()[t].vertex( v ).position.y;
				vertex.z = surface->get_triangles()[t].vertex( v ).position.z;
			}
		}

		rtcUnmapBuffer( scene, geom_id, RTC_VERTEX_BUFFER );

		// vytváření indexů vrcholů pro jednotlivé trojúhelníky
		embree_structs::Triangle * triangles = static_cast< embree_structs::Triangle * >(
			rtcMapBuffer( scene, geom_id, RTC_INDEX_BUFFER ) );

		for ( int t = 0, v = 0; t < surface->no_triangles(); ++t )
		{
			embree_structs::Triangle & triangle = triangles[t];

			triangle.v0 = v++;
			triangle.v1 = v++;
			triangle.v2 = v++;
		}

		rtcUnmapBuffer( scene, geom_id, RTC_INDEX_BUFFER );

		/*embree_structs::Normal * normals = static_cast< embree_structs::Normal * >(
			rtcMapBuffer( scene, geom_id, RTC_USER_VERTEX_BUFFER0 ) );
			rtcUnmapBuffer( scene, geom_id, RTC_USER_VERTEX_BUFFER0 );*/
	}

	rtcCommit( scene );	

	float width = 1024;
	float height = 720;
	float fov_y = 42.185f;
	//float fov_y = 120.0f;
	// Avenger.
	//Vector3 viewFrom = { -140.0f, -175.0f, 110.0f };
	//Vector3 viewAt = { 0.0f, 0.0f, 40.0f };
	// Sphere.
	Vector3 viewFrom = { 3.0f, 0.0f, 0.0f };
	Vector3 viewAt = { 0.0f, 0.0f, 0.0f };

	Camera camera = Camera(width, height, viewFrom, viewAt, DEG2RAD(fov_y));
	OmniLight light = OmniLight({-170, 170, 220}, { 1,1,1 }, { 1,1,1 }, { 1,1,1 });
	
	camera.Print();

	// TODO *** ray tracing ****
	//test( scene, surfaces );
	//GenericRaytraceWithEmbree(scene, camera, surfaces);

	std::string cubePath = "../../data/cubemap1/";
	std::string fileType = ".jpg";
	CubeMap cubeMap = { std::vector<std::string> {
						cubePath + "posx" + fileType,
						cubePath + "negx" + fileType,
						cubePath + "posy" + fileType,
						cubePath + "negy" + fileType,
						cubePath + "posz" + fileType,
						cubePath + "negz" + fileType, }};


	Renderer renderer = Renderer();
	ShaderWhite shaderWhite = ShaderWhite(scene, camera, surfaces, light, cubeMap);
	ShaderLambert shaderLambert = ShaderLambert(scene, camera, surfaces, light, cubeMap);
	ShaderNormal shaderNormal = ShaderNormal(scene, camera, surfaces, light, cubeMap);
	ShaderPhong shaderPhong = ShaderPhong(scene, camera, surfaces, light, cubeMap);
	PathTrace pathTracerTest = PathTrace(scene, camera, surfaces, light, cubeMap);
	//pathTracerTest.samples = 200;

	
	// Refraction
	// camera z bodu 3,0,0 na 0,0,0

	//shaderPhong.reflectionIntensity = 0.9f;
	bool superSampling = false;
	renderer.Render(shaderPhong, superSampling);

	while (true)
	{
		int c = cv::waitKey(0);

		Vector3 v = camera.view_at();
		Vector3 f = camera.view_from();
		float moveBy = 50.0f;
		if (c == 2490368)
		{
			v.y += moveBy;
			f.y -= moveBy;
			printf("Up\n");
		} // up
		else if (c == 2621440)
		{
			v.y -= moveBy;
			f.y += moveBy;
			printf("Down\n");
		}  // down
		else if (c == 2424832)
		{
			v.x -= moveBy;
			f.x += moveBy;
			printf("Left\n");
		}  //  left
		else if (c == 2555904)
		{
			v.x += moveBy;
			f.x -= moveBy;
			printf("Right\n");
		}  // right
		else if (c == 32) 
		{
			// space to exit
			break;
		}

		v.Print();
		printf("\n");


		camera.set_view(v, f);
		renderer.Render(shaderPhong, superSampling);
	}
	
	rtcDeleteScene( scene ); // zrušení Embree scény

	SafeDeleteVectorItems<Material *>( materials );
	SafeDeleteVectorItems<Surface *>( surfaces );

	rtcDeleteDevice( device ); // Embree zařízení musíme také uvolnit před ukončením aplikace

	return 0;
}
