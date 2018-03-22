#include "raytracer.h"

#include "image.h"
#include "light.h"
#include "material.h"
#include "triple.h"
#include "objloader.h"
#include "vertex.h"

// =============================================================================
// -- Include all your shapes here ---------------------------------------------
// =============================================================================

#include "shapes/sphere.h"
#include "shapes/triangle.h"
#include "shapes/plane.h"
#include "shapes/torus.h"

// =============================================================================
// -- End of shape includes ----------------------------------------------------
// =============================================================================

#include "json/json.h"

#include <exception>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;        // no std:: required
using json = nlohmann::json;

bool Raytracer::parseObjectNode(json const &node)
{
    ObjectPtr obj = nullptr;

// =============================================================================
// -- Determine type and parse object parametrers ------------------------------
// =============================================================================


    if (node["type"] == "sphere")
    {
        Point pos(node["position"]);
		pos    *= resolutionMultiplier;
        double radius = node["radius"];
        radius *= resolutionMultiplier;
        string name = node["comment"];

        Vector rot(1.0,1.0,1.0);
        if (node.isInNode("rotation"))
            rot = Vector(node["rotation"]);
        size_t angle = 0;
        if (node.isInNode("angle"))
            angle = node["angle"];
        if (rot.x == 0.0 && rot.y == 0.0 && rot.z == 0.0) {
            // zero vector, not allowed, return unrotated sphere
            angle = 0;
            rot = Vector(1.0, 1.0, 1.0);
        }
        obj = ObjectPtr(new Sphere(pos, radius, rot, angle));
    }
    else if (node["type"] == "triangle")
    {
		Point point1(node["point1"]);
		point1 *= resolutionMultiplier;
		Point point2(node["point2"]);
		point2 *= resolutionMultiplier;
		Point point3(node["point3"]);
		point3 *= resolutionMultiplier;
		obj = ObjectPtr(new Triangle(point1, point2, point3));
    }
	else if (node["type"] == "plane")
	{
		Point pos(node["position"]);
		pos  *= resolutionMultiplier;
		Vector Normal(node["normal"]);
		size_t size(node["size"]);
		size *= resolutionMultiplier;
		obj = ObjectPtr(new Plane(pos, Normal, size));
	}
	else if (node["type"] == "torus")
	{
		Point center(node["center"]);
		center  *= resolutionMultiplier;
		Vector axis(node["axis"]);
		size_t innerRadius(node["innerRadius"]);
		size_t outerRadius(node["outerRadius"]);
		innerRadius *= resolutionMultiplier;
		outerRadius *= resolutionMultiplier;
		obj = ObjectPtr(new Torus(center, axis, innerRadius, outerRadius));
	}
    else if (node["type"] == "mesh")
    {
        string filename = node["model"];
        OBJLoader objLoader = OBJLoader(filename);
        objLoader.unitize();
		float scale = node["scale"];
        objLoader.scale(scale * resolutionMultiplier);
        Point pos(node["position"]);
		pos *= resolutionMultiplier;
        objLoader.move(pos.x, pos.y, pos.z);

        vector<Vertex> vertices = objLoader.vertex_data();
        for (size_t i = 0; i < objLoader.numTriangles() * 3; i = i + 3) {
            Point a(vertices[i].x, vertices[i].y, vertices[i].z);
            Point b(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z);
            Point c(vertices[i + 2].x, vertices[i + 2].y, vertices[i + 2].z);
            obj = ObjectPtr(new Triangle(a, b, c));
            obj->material = parseMaterialNode(node["material"]);
            scene.addObject(obj);
        }
        return true;
    }
    else
    {
        cerr << "Unknown object type: " << node["type"] << ".\n";
    }

// =============================================================================
// -- End of object reading ----------------------------------------------------
// =============================================================================

    if (!obj)
        return false;

	bool shadow = true;
	if (node.isInNode("shadow"))
		shadow = node["shadow"];
	obj->shadow  = shadow;

    // Parse material and add object to the scene
    if (node["material"].isInNode("color"))
        obj->material = parseMaterialNode(node["material"]);
    else
    {
        obj->material = parseMaterialNodeTexture(node["material"]);
        string texturePath = "../scenes/";
		json matNode = node["material"];
        texturePath += matNode["texture"];
        Image texture(texturePath);
        obj->texture = texture;
    }
    scene.addObject(obj);
    return true;
}

Light Raytracer::parseLightNode(json const &node) const
{
    Point pos(node["position"]);
	pos *= resolutionMultiplier;
    Color col(node["color"]);
    return Light(pos, col);
}

Material Raytracer::parseMaterialNode(json const &node) const
{
    double ka = node["ka"];
    double kd = node["kd"];
    double ks = node["ks"];
    double n  = node["n"];
    Color color(node["color"]);
    return Material(color, ka, kd, ks, n, false);
}

Material Raytracer::parseMaterialNodeTexture(json const &node) const
{
    Color color(0.0,0.0,0.0);
    double ka = node["ka"];
    double kd = node["kd"];
    double ks = node["ks"];
    double n  = node["n"];
    return Material(color, ka, kd, ks, n, true);
}

bool Raytracer::readScene(string const &ifname)
try
{
    // Read and parse input json file
    ifstream infile(ifname);
    if (!infile) throw runtime_error("Could not open input file for reading.");
    json jsonscene;
    infile >> jsonscene;

// =============================================================================
// -- Read your scene data in this section -------------------------------------
// =============================================================================

    resolutionMultiplier = jsonscene.isInNode("ResolutionMultiplier") ? static_cast<float>(jsonscene["ResolutionMultiplier"]) : 1;
    aspectRatio          = jsonscene.isInNode("AspectRatio")          ? static_cast<float>(jsonscene["AspectRatio"])          : 1;

    Point eye(jsonscene["Eye"]);
    eye *= resolutionMultiplier;
    scene.setEye(eye);
    auto null = json::value_type::value_t::null;

    scene.setShadows(jsonscene["Shadows"] == null ? false : jsonscene["Shadows"] == true ? true : false);
    scene.setMaxRecDepth(jsonscene["MaxRecursionDepth"] != null ? static_cast<size_t>(jsonscene["MaxRecursionDepth"]) : 0);
    scene.setSuperSampFac(jsonscene["SuperSamplingFactor"] != null ? static_cast<size_t>(jsonscene["SuperSamplingFactor"]) : 1);

    for (auto const &lightNode : jsonscene["Lights"])
        scene.addLight(parseLightNode(lightNode));

    unsigned objCount = 0;
    for (auto const &objectNode : jsonscene["Objects"])
        if (parseObjectNode(objectNode))
            ++objCount;

    cout << "Parsed " << objCount << " objects.\n";

// =============================================================================
// -- End of scene data reading ------------------------------------------------
// =============================================================================

    return true;
}
	catch (exception const &ex)
	{
		cerr << ex.what() << '\n';
		return false;
	}

void Raytracer::renderToFile(string const &ofname)
{
    // TODO: the size may be a settings in your file
    float xMul = (aspectRatio < 1 ? 1 : aspectRatio) * resolutionMultiplier;
    float yMul = (aspectRatio < 1 ? 1 / aspectRatio : 1) * resolutionMultiplier;
    Image img(400 * xMul, 400 * yMul);
    cout << "Tracing...\n";
    scene.render(img);
    cout << "Writing image to " << ofname << "...\n";
    img.write_png(ofname);
    cout << "Done.\n";
}
