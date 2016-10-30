#pragma once

#include <vector>
#include <memory>
#include <map>
#include <string>

#include "ActorComponent.h"
#include "IPhysicsComponent.h"

#include "../../game_view/rendering/Geometry.h"
#include "../physics/Spring.h"
#include "../physics/ClothParticle.h"
#include "../physics/Force.h"
#include "../physics/ClothTriangle.h"

using std::vector;
using std::map;
using std::shared_ptr;
using std::pair;
using std::string;

using namespace Game_View;

namespace Game_Logic
{
    typedef vector<StrongSpringPtr> Springs;
    typedef vector<StrongClothParticlePtr> Particles;
    typedef vector<StrongTrianglePtr> Triangles;
    
    /**
     * Component that will turn the current
     * actor into a cloth. The component
     * will create the control points that will
     * be used by the shaders to create B-splines
     */
    class ClothComponent : public ActorComponent, public IPhysicsComponent
    {
    private:
        /**
         * TODO
         */
        std::vector<vec3> m_normalCalculationData;
        
        /**
         * The grid positions of the points that are going to be fixed
         */
        vector<glm::vec2> m_fixedPoints;
        
        /**
         * A pointer to the geometry the cloth component is manipulating
         */
        StrongGeometryPtr m_pGeometry;
        
        /**
         * The data structure that is holding all the springs
         * in the current cloth geometry.
         */
        Springs m_springs;
        
        /**
         * The triangles of the cloth geometry, to be able to
         * calculate the normals of a triangle to apply the force
         * correctly
         */
        Triangles m_triangles;
        
        /**
         * Data structure to store the vertices of the geometry
         * that is going to be created in the ClothComponent
         * This structure stores the four vertices that are needed
         * to create a trapezoid.
         */
        vector<glm::vec3> m_vertices;

        /**
         * Spring constants for the structural springs
         */
        float m_kdStruct, m_ksStruct;
        
        /**
         * Spring constants for the bending springs
         */
        float m_kdBend, m_ksBend;

        /**
         * Spring constants for the shearing springs
         */
        float m_kdShear, m_ksShear;
        
        /**
         * Dimensions of the grid that is the base for the
         * geometry that will be created in the cloth component
         */
        int m_numParticlesWidth, m_numParticlesHeight;
        
        /**
         * The name of the material the cloth is using.
         * This needs to be initialized in the cloth component,
         * because the cloth's geometry is created in this class
         */
        std::string m_materialName;
        
        /**
         * The particles that form the cloth. The particles are the vertices
         * of the geometry
         */
        Particles m_particles;
        
        /**
         * how many iterations of constraint satisfaction each frame (more is rigid, less is soft)
         */
        unsigned int m_constraintIterations;
        
        /**
         * Damping for the spring the cloth uses
         */
        float m_damping;
        
        /**
         * The mass of every particle in the current cloth simulation
         * default value 1.0f
         */
        float m_massPerParticle;

        /**
         * Creates an uniform grid from that fits into the 4 points
         * given in the xml file
         */
        void CreateGeometryFromXMLElement(StrongXMLElementPtr);
        
        /**
         * Initializes the spring constants by the given xml element
         */
        void InitSprings(StrongXMLElementPtr);
        
        /**
         * Creates the springs from the current geometry,
         * by setting three different kinds of springs depending
         * on the distance to the neighbours of each particle
         */
        void CreateSpringsFromGeometry();
        
        /**
         * Adds a triangle to the according data structure
         * to be able to calculate area forces more easily later on
         */
        void AddTriangle(
                const unsigned int&,
                const unsigned int&,
                const unsigned int&,
                std::vector<unsigned int>&);
        
        /**
         * Makes sure that the saved constraints are satisfied
         */
        void EnforceConstraints(const float&);
        
        /**
         * Iterates over all particles in the current cloth simulation
         * and calls their integrate method
         */
        void IntegrateParticles(const float&);
        
        /**
         * Initialize the fix points
         */
        void MakeFixPoints();
        
        /**
         * Make the given cloth particle a fix point
         */
        void MakeFixPoint(StrongClothParticlePtr);
        
        /**
         * Apply the relevant forces for the cloth,
         * such as gravity and wind
         */
        void ApplyForces(const mat4&, const float&);
        
        /**
         * Applies the given force to every particle
         * in the current cloth geometry. The bool parameter
         * indicates whether the force is mass-independent (such as gravity)
         * or not.
         */
        void ApplyForceToAllParticles(StrongForcePtr, const mat4&, const float&, bool);
        
        /**
         * Applies the given force to the given set of particles.
         * Method has the same effect as the method ApplyForceToAllParticles,
         * except this method only applies the force to the given set of particles
         */
        void ApplyForceToParticles(const vec3&, Particles&, bool);
        
        /**
         * Applies the given force to all triangles in the current cloth geometry,
         * by calculating the normal of each triangle first and calculating the
         * direction of the force on this triangle by using the normal in the calculation.
         */
        void ApplyForceToAllTriangles(StrongForcePtr, const mat4&, const float&);
        
        /**
         * Calculates the normal of the given triangle.
         */
        const vec3 CalculateNormalOfTriangle(StrongTrianglePtr) const;
        
        /**
         * Initialize the fix points given by the XML data
         */
        void InitFixedPoints(StrongXMLElementPtr);
        
        /**
         * Creates a spring at the given position in the grid (x, y),
         * with the given spring constants ks and kd
         */
        void MakeSpring(
            unsigned int, unsigned int, const float, const float);
        
        /**
         * Starts to create the geometry for the cloth component
         * based on the given vertices
         */
        void CreateGeometry();
        
        /**
         * Assigns the recent created geometry to the
         * underlying actor
         */
        void AssignGeometryToActor();
        
        /**
         * Enforces the given spring for the given time period
         */
        void EnforceConstraint(const float&, StrongSpringPtr);
        
        /**
         * Integrates the acceleration of the spring to get
         * the new position
         */
        void ParticleTimeStep(const float&, StrongClothParticlePtr);
        
        /**
         * Returns the particle that can be found at grid position (x, y)
         */
        unsigned int GetParticleIndex(unsigned int, unsigned int);

        /**
         * TODO
         */
		void GetParticleCoords(unsigned int idx, unsigned int& x, unsigned int& y);
        
        /**
         * Creates all the structural springs existing in the geometry
         */
        void CreateStructuralSprings();
        
        /**
         * Creates all the bending springs existing in the geometry
         */
        void CreateBendSprings();
        
        /**
         * Creates all the shear springs existing in the geometry
         */
        void CreateShearSprings();
        
        /**
         * Creates the adjacent springs for all particles with the given distance and
         * spring constants
         */
        void CreateAdjacentSprings(const unsigned int&, const float&, const float&);
        
        /**
         * Resets the acceleration of every particle in the cloth
         * to (0, 0, 0)
         */
        void ResetAllAccelerations();
        
        /**
         * The force that is acting on all particles together will be transferred
         * to the sail component of the actor - if it has a sail component
         */
        void TransferTotalForceToSailComponents(const float&);
        
        /**
         * Summarizes the accelerations on every particle in the cloth
         * and returns the sum
         */
        const vec3 GetTotalForceOnCloth() const;
        
        /**
         * Calculates the normal vector for the geometry before
         * its getting transformed
         */
        const vec3 CalculateNormalForClothComponent() const;
        
        /**
         * Assigns the material that were set in the xml
         */
        void AssignClothMaterial() const;
        
        /**
         * Creates the all vertices of the geometry based on the
         * four vertices given from the xml
         */
        void CreateVertices();
        
        /**
         * Creates the indices to form triangles with the previous
         * created vertices
         */
        void CreateIndices();

        /**
         * TODO
         */
		void UpdateNormals();
        
        /**
         * Returns the inverse transform matrix of the parent
         * scene node to transform the wind direction into the
         * local coordinate system
         */
        const mat4 GetInverseTransformOfSceneNode();
        
    public:
        ClothComponent() = default;
        
        virtual ~ClothComponent();
        
        /**
         * Initializes the current cloth component
         * by creating the geometry and setting the constraints
         */
        virtual void VInit();
        
        virtual string VGetIdentifier() const
        {
            return "clothComponent";
        }
        
        /*
         * Initializes the cloth component by the given XML element
         */
        virtual void VFetchData(StrongXMLElementPtr);
        
        virtual void VUpdate(const unsigned int);
        
        /**
         * Returns a pointer to the recent created geometry
         */
        StrongGeometryPtr GetGeometry() const
        {
            return m_pGeometry;
        }
        
        /**
         * Remove the fix point that can be found at the given grid position (x, y)
         */
        void RemoveFixPoint(const glm::vec2);
        
        /**
         *  TODO
         */
        void TowPoint(const glm::vec2, const glm::vec3, const float&);
    };
    
}