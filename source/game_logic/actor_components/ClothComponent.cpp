#include "ClothComponent.h"
#include "RenderComponent.h"
#include "SailComponent.h"
#include "../actor_system/Actor.h"
#include "../physics/ClothParticle.h"
#include "../../game_view/scene/Scene.h"
#include "../../application_layer/core/GameCore.h"
#include <string>

using namespace Game_Logic;


ClothComponent::~ClothComponent()
{
}

void ClothComponent::VInit()
{
    
    CreateGeometry();
    CreateSpringsFromGeometry();
    AssignGeometryToActor();
    AssignClothMaterial();
    MakeFixPoints();
}

void ClothComponent::AssignGeometryToActor()
{
    StrongActorPtr pActor(m_pActor);
    
    if(!pActor->HasComponent("renderComponent"))
    {
        return ;
    }
    
     shared_ptr<RenderComponent> pRenderComponent =
                        std::static_pointer_cast<RenderComponent>(
                                        pActor->GetComponent("renderComponent"));

    pRenderComponent->SetGeometry(m_pGeometry);
}

void ClothComponent::ApplyForces(
                const mat4& sceneNodeTransform, const float& time)
{
    for(StrongClothParticlePtr pParticle : m_particles)
    {
        vec3 pos =
                m_pGeometry->GetVertexAtIndex(pParticle->GetIndex()).Position;
        vec3 pos_old =
                pParticle->GetOldPosition();
        vec3 vel = (pos - pos_old) / time;
        
        pParticle->AddForce(vel * m_damping);
    }
    
    for(StrongForcePtr pForce : g_pGameApp->GetCurrentScene()->GetForces())
    {
        if(pForce->IsAreaForce())
        {
            ApplyForceToAllTriangles(
                        pForce, sceneNodeTransform, time);
        }
        else
        {
            ApplyForceToAllParticles(
                        pForce,
                        sceneNodeTransform,
                        time,
                        pForce->IsMassIndependent()
                        );
        }
    }
}

void ClothComponent::ApplyForceToAllTriangles(
                                    StrongForcePtr pForce,
                                    const mat4& transform,
                                    const float& time)
{
    vec4 temp = transform * vec4(pForce->GetDirection(), 0);
    vec3 forceDirection = vec3(temp.x, temp.y, temp.z);
    
    for(StrongTrianglePtr pTriangle : m_triangles)
    {
        vec3 normal = CalculateNormalOfTriangle(pTriangle);
        vec3 d = glm::normalize(normal);
        vec3 force =
            normal * (glm::dot(d, forceDirection)) * pForce->GetStrength();

        ApplyForceToParticles(force, pTriangle->GetParticles(), false);
    }
}

const vec3 ClothComponent::CalculateNormalOfTriangle(StrongTrianglePtr pTriangle) const
{
    Particles particles = pTriangle->GetParticles();
    
    vec3 pos1 =
            m_pGeometry->GetVertexAtIndex(particles[0]->GetIndex()).Position;
    vec3 pos2 =
            m_pGeometry->GetVertexAtIndex(particles[1]->GetIndex()).Position;
    vec3 pos3 =
            m_pGeometry->GetVertexAtIndex(particles[2]->GetIndex()).Position;
    
    vec3 v1 = pos2 - pos1;
    vec3 v2 = pos3 - pos1;
    
    return glm::cross(v1, v2);
}

void ClothComponent::ApplyForceToParticles(
                        const vec3& force,
                        Particles& particles,
                        bool multiplyForceWithMass)
{

    for(StrongClothParticlePtr pParticle : particles)
    {
        if(!multiplyForceWithMass)
        {
            pParticle->AddMassIndependentForce(force);
        }
        else
        {
            pParticle->AddForce(force);
        }
    }
}

void ClothComponent::AddTriangle(
                const unsigned int& index1,
                const unsigned int& index2,
                const unsigned int& index3,
                std::vector<unsigned int>& indices)
{
    
    indices.push_back(index1);
    indices.push_back(index2);
    indices.push_back(index3);
    
    m_triangles.push_back(
                        StrongTrianglePtr(
                          new ClothTriangle(
                                            m_particles[index1],
                                            m_particles[index2],
                                            m_particles[index3]
                                            )));
}

void ClothComponent::ApplyForceToAllParticles(
                            StrongForcePtr pForce,
                            const mat4& transform,
                            const float& time,
                            bool multiplyForceWithMass)
{
    vec4 force =
            transform * vec4(pForce->GetDirection(), 0) * pForce->GetStrength() * time;
    
    
    ApplyForceToParticles(
                    vec3(force.x, force.y, force.z),
                    m_particles,
                    multiplyForceWithMass
                    );
}

void ClothComponent::CreateGeometryFromXMLElement(StrongXMLElementPtr pXmlElement)
{
    m_pGeometry = StrongGeometryPtr(new Geometry());
    m_pGeometry->SetIsVolatile(true);
	m_pGeometry->SetBackFaceCulling(false);
    
    for(StrongXMLElementPtr pVertexElement : pXmlElement->GetChildren())
    {
        m_vertices.push_back(glm::vec3(
                                       std::stod(pVertexElement->GetValueOfAttribute("x")),
                                       std::stod(pVertexElement->GetValueOfAttribute("y")),
                                       std::stod(pVertexElement->GetValueOfAttribute("z"))
                                       ));
    }
}

void ClothComponent::CreateGeometry()
{
    if(m_vertices.size()<4)
    {
        return ;
    }
    
    CreateVertices();
    CreateIndices();
}

void ClothComponent::CreateIndices()
{
    vector<unsigned int> indices;
    
    for(int x = 0; x < m_numParticlesWidth-1; x++)
    {
        for(int y = 0; y < m_numParticlesHeight-1; y++)
        {
            AddTriangle(
                        GetParticleIndex(x+1, y),
                        GetParticleIndex(x, y),
                        GetParticleIndex(x, y+1),
                        indices
                        );
            
            AddTriangle(
                        GetParticleIndex(x+1, y+1),
                        GetParticleIndex(x+1, y),
                        GetParticleIndex(x, y+1),
                        indices
                        );
        }
    }
    
    m_pGeometry->SetIndices(indices);
}

void ClothComponent::CreateVertices()
{
    glm::vec3 normal =
                CalculateNormalForClothComponent();
    vector<VertexData> vertices;
    const float materialIndex = 0.0f;
    int count = 0;
    int totalParticles = m_numParticlesWidth * m_numParticlesHeight;
    
    vertices.resize(totalParticles);
    m_particles.resize(totalParticles);
    
    for(int j = 0; j < m_numParticlesHeight; j++)
    {
        for(int i = 0; i < m_numParticlesWidth; i++)
        {
            VertexData vertexData;
            
            float u = float(i)/(m_numParticlesWidth-1);
            float v = float(j)/(m_numParticlesHeight-1);
            
            vec3 k = glm::mix(m_vertices[1], m_vertices[0], u);
            vec3 l = glm::mix(m_vertices[3], m_vertices[2], u);
            
            vertexData.Position = glm::mix(k, l, v);
            vertexData.Normal = normal;
            vertexData.MaterialIndex = materialIndex;
            vertexData.UV = vec2(u, v);
            
            StrongClothParticlePtr pClothParticle(
                                                  new ClothParticle(
                                                                    count,
                                                                    vertexData.Position,
                                                                    m_massPerParticle,
                                                                    m_damping
                                                                    ));
            
            m_particles[count] = StrongClothParticlePtr(pClothParticle);
            vertices[count] = vertexData;
            count++;
        }
    }
    
    m_pGeometry->SetAllVertexData(vertices);
}

void ClothComponent::AssignClothMaterial() const
{
    StrongActorPtr pActor(m_pActor);
    
    if(!pActor->HasComponent("renderComponent"))
    {
        return ;
    }
    
    RenderComponent* pRenderComponent =
            dynamic_cast<RenderComponent*>(
                    pActor->GetComponent("renderComponent").get());
    
    Material material;
    
    material.LoadFromFile(m_materialName);
   
    pRenderComponent->AddMaterial(material);
}

const vec3 ClothComponent::CalculateNormalForClothComponent() const
{
    // x direction
    vec3 v1 = m_vertices[1] - m_vertices[0];
    // y direction
    vec3 v2 = m_vertices[3] - m_vertices[2];
    
    return glm::normalize(glm::cross(v2, v1));
}

void ClothComponent::MakeSpring(
            unsigned int index1,
            unsigned int index2,
            const float ks,
            const float kd)
{
    VertexData particle1 =
            m_pGeometry->GetVertexAtIndex(index1);
    VertexData particle2 =
            m_pGeometry->GetVertexAtIndex(index2);
    double rest_length =
            glm::length(particle1.Position-particle2.Position);
    
    m_springs.push_back(
            StrongSpringPtr(
                new Spring(
                           index1,
                           index2,
                           rest_length,
                           ks,
                           kd
                           )));
}

void ClothComponent::CreateStructuralSprings()
{
    CreateAdjacentSprings(1, m_ksStruct, m_kdStruct);
}

void ClothComponent::CreateBendSprings()
{
    CreateAdjacentSprings(2, m_ksBend, m_kdBend);
}

void ClothComponent::CreateAdjacentSprings(
        const unsigned int& distance, const float& ks, const float& kd)
{
    for (int x = 0; x < m_numParticlesWidth; x++)
    {
        for (int y = 0; y < m_numParticlesHeight; y++)
        {
            if(x < m_numParticlesWidth-distance)
            {
                MakeSpring(
                    GetParticleIndex(x, y),
                    GetParticleIndex(x+distance, y), ks, kd);
            }
            
            if(y < m_numParticlesHeight-distance)
            {
                MakeSpring(
                    GetParticleIndex(x, y),
                    GetParticleIndex(x, y+distance), ks, kd);
            }
            
            if(x >= distance)
            {
                MakeSpring(
                    GetParticleIndex(x, y),
                    GetParticleIndex(x-distance, y), ks, kd);
            }
            
            if(y >= distance)
            {
                MakeSpring(
                    GetParticleIndex(x, y),
                    GetParticleIndex(x, y-distance), ks, kd);
            }
        }
    }
}

void ClothComponent::CreateShearSprings()
{
    for (int x = 0; x < m_numParticlesWidth; x++)
    {
        for (int y = 0; y < m_numParticlesHeight; y++)
        {
            if(x < m_numParticlesWidth-1 && y>0)
            {
                MakeSpring(GetParticleIndex(x, y), GetParticleIndex(x+1, y-1), m_ksShear, m_kdShear);
            }
            
            if(x > 0 && y>0)
            {
                MakeSpring(GetParticleIndex(x, y), GetParticleIndex(x-1, y-1), m_ksShear, m_kdShear);
            }
            
            if(y < m_numParticlesHeight-1 && x>0)
            {
                MakeSpring(GetParticleIndex(x, y), GetParticleIndex(x-1, y+1), m_ksShear, m_kdShear);
            }
            
            if(x < m_numParticlesWidth-1 && y < m_numParticlesHeight-1)
            {
                MakeSpring(GetParticleIndex(x, y), GetParticleIndex(x+1, y+1), m_ksShear, m_kdShear);
            }
        }
    }
}

void ClothComponent::CreateSpringsFromGeometry()
{
    CreateStructuralSprings();
    CreateShearSprings();
    CreateBendSprings();
}

unsigned int ClothComponent::GetParticleIndex(unsigned int x, unsigned int y)
{
    return y * m_numParticlesWidth + x;
}

void ClothComponent::GetParticleCoords(unsigned int idx, unsigned int& x, unsigned int& y)
{
	x = idx % m_numParticlesWidth;
	y = idx / m_numParticlesWidth;
}

void ClothComponent::MakeFixPoints()
{
    for(glm::vec2 fixPoint : m_fixedPoints)
    {
        StrongClothParticlePtr pParticle =
                m_particles[GetParticleIndex(fixPoint.x, fixPoint.y)];
        
        MakeFixPoint(pParticle);
    }
}

void ClothComponent::MakeFixPoint(StrongClothParticlePtr pParticle)
{
    if(pParticle==nullptr)
    {
        return ;
    }
    
    pParticle->MakeUnmovable();
}

void ClothComponent::VFetchData(StrongXMLElementPtr pXmlElement)
{
    if(pXmlElement->HasChildrenNamed("damping"))
    {
        StrongXMLElementPtr pDampingElement =
                    pXmlElement->FindFirstChildNamed("damping");
        
        m_damping =
                std::stof(pDampingElement->GetValueOfAttribute("value"));
    }
    
    if(pXmlElement->HasChildrenNamed("constraintIterations"))
    {
        StrongXMLElementPtr pConstraintIterationsElement =
                pXmlElement->FindFirstChildNamed("constraintIterations");
        
        m_constraintIterations =
                        std::stoi(pConstraintIterationsElement->GetValueOfAttribute("value"));
    }
    
    if(pXmlElement->HasChildrenNamed("numParticles"))
    {
        StrongXMLElementPtr pNumParticlesElement =
                pXmlElement->FindFirstChildNamed("numParticles");
        
        if(pNumParticlesElement->HasChildrenNamed("width"))
        {
            StrongXMLElementPtr pWidthElement =
                pNumParticlesElement->FindFirstChildNamed("width");
            
            m_numParticlesWidth = std::stoi(pWidthElement->GetValueOfAttribute("value"));
        }
        
        if(pNumParticlesElement->HasChildrenNamed("height"))
        {
            StrongXMLElementPtr pHeightElement =
                pNumParticlesElement->FindFirstChildNamed("height");
            
            m_numParticlesHeight = std::stoi(pHeightElement->GetValueOfAttribute("value"));
        }
    }
    
    if(pXmlElement->HasChildrenNamed("totalMass"))
    {
        StrongXMLElementPtr pMassPerParticle =
                pXmlElement->FindFirstChildNamed("totalMass");
        
        m_massPerParticle =
                std::stof(pMassPerParticle->GetValueOfAttribute("value"))
                / (m_numParticlesHeight * m_numParticlesWidth);
    }
    
    if(pXmlElement->HasChildrenNamed("fixedPoints"))
    {
        StrongXMLElementPtr pFixedParticlesElement =
                pXmlElement->FindFirstChildNamed("fixedPoints");
        
        InitFixedPoints(pFixedParticlesElement);
    }
    
    if(pXmlElement->HasChildrenNamed("renderMaterial"))
    {
        StrongXMLElementPtr pMaterialElement =
                    pXmlElement->FindFirstChildNamed("renderMaterial");
        
        m_materialName = pMaterialElement->GetValueOfAttribute("name");
    }
    
    if(pXmlElement->HasChildrenNamed("springs"))
    {
        StrongXMLElementPtr pSpringsElement =
                        pXmlElement->FindFirstChildNamed("springs");
        
        InitSprings(pSpringsElement);
    }
    
    if(pXmlElement->HasChildrenNamed("vertices"))
    {
        CreateGeometryFromXMLElement(
                                     pXmlElement->FindFirstChildNamed("vertices"));
    }
}

void ClothComponent::InitSprings(
                        StrongXMLElementPtr pSpringsElement)
{
    if(pSpringsElement->HasChildrenNamed("structural"))
    {
        StrongXMLElementPtr pStructuralSpringElement =
                    pSpringsElement->FindFirstChildNamed("structural");
        
        m_kdStruct =
            std::stof(pStructuralSpringElement->GetValueOfAttribute("kd"));
        m_ksStruct =
            std::stof(pStructuralSpringElement->GetValueOfAttribute("ks"));
    }
    
    if(pSpringsElement->HasChildrenNamed("bending"))
    {
        StrongXMLElementPtr pBendSpringElement =
                    pSpringsElement->FindFirstChildNamed("bending");
        
        m_kdBend =
            std::stof(pBendSpringElement->GetValueOfAttribute("kd"));
        m_ksBend =
            std::stof(pBendSpringElement->GetValueOfAttribute("ks"));
    }

    if(pSpringsElement->HasChildrenNamed("shear"))
    {
        StrongXMLElementPtr pShearSpringElement =
                    pSpringsElement->FindFirstChildNamed("shear");
        
        m_kdShear =
                std::stof(pShearSpringElement->GetValueOfAttribute("kd"));
        m_ksShear =
                std::stof(pShearSpringElement->GetValueOfAttribute("ks"));
    }
}

void ClothComponent::InitFixedPoints(StrongXMLElementPtr pFixedParticlesElement)
{
    int i = 0;
    
    StrongXMLElementPtr pChild =
                    pFixedParticlesElement->FindChildNamed("fixedPoint", i);
    
    while(pChild!=nullptr)
    {
        m_fixedPoints.push_back(
                glm::vec2(
                        std::stoi(pChild->GetValueOfAttribute("x")),
                        std::stoi(pChild->GetValueOfAttribute("y"))
                        ));
        
        // by given line
        
        pChild =
            pFixedParticlesElement->FindChildNamed("fixedPoint", ++i);
    }
}

const mat4 ClothComponent::GetInverseTransformOfSceneNode()
{
    StrongActorPtr pActor = StrongActorPtr(m_pActor);
    StrongSceneNodePtr pSceneNode =
                    g_pGameApp->GetCurrentScene()->
                        GetSceneGraph()->FindNodeWithActor(pActor->GetActorId());
    
    const mat4 matrix = pSceneNode->VGetTransformationMatrix();
    
    return glm::inverse(matrix);
}

void ClothComponent::VUpdate(const unsigned int deltaMilliseconds)
{
    if(deltaMilliseconds==0)
    {
        return ;
    }
    
    const unsigned int millisecondsPerSecond = 1000;
    const float time = glm::clamp(deltaMilliseconds, 1u, 15u) / (1000.0f * m_constraintIterations);
    const mat4 sceneNodeTransform =
                            GetInverseTransformOfSceneNode();
    
    for(int i=0; i < m_constraintIterations; i++) // iterate over all constraints several times
    {
        ApplyForces(sceneNodeTransform, time);
        EnforceConstraints(time);
        IntegrateParticles(time);
        TransferTotalForceToSailComponents(time);
        ResetAllAccelerations();
    }

	UpdateNormals();
}

void ClothComponent::TransferTotalForceToSailComponents(const float& time)
{
    StrongActorPtr pActor(m_pActor);
    
    if(!pActor->HasComponent("sailComponent"))
    {
        return ;
    }
    
    SailComponent* pSailComponent =
                dynamic_cast<SailComponent*>(
                                  pActor->GetComponent("sailComponent").get());
    
    pSailComponent->ApplyForce(GetTotalForceOnCloth());
}

void ClothComponent::EnforceConstraints(const float& time)
{
    for(StrongSpringPtr pSpring : m_springs)
    {
        EnforceConstraint(time, pSpring);
    }
}

void ClothComponent::IntegrateParticles(const float& time)
{
    for(StrongClothParticlePtr pParticle : m_particles)
    {
        ParticleTimeStep(time, pParticle);
    }
}

void ClothComponent::ResetAllAccelerations()
{
    for(StrongClothParticlePtr pParticle : m_particles)
    {
        pParticle->ResetAcceleration();
    }
}

void ClothComponent::ParticleTimeStep(
                const float& dt, StrongClothParticlePtr pParticle)
{
    if(!pParticle->IsMoveable())
    {
        return ;
    }
    
    float timeFactor = pow(dt, 2);
    
    vec3 temp =
            m_pGeometry->GetVertexAtIndex(pParticle->GetIndex()).Position;
    vec3 acc = pParticle->GetAcceleration();
    vec3 position =
            temp + (temp-pParticle->GetOldPosition()) + acc*timeFactor;
    
    pParticle->SetOldPosition(temp);
    m_pGeometry->SetPositionAtIndex(pParticle->GetIndex(), position);
    
    pParticle->ResetAcceleration();
}

void ClothComponent::EnforceConstraint(
                    const float& dt, StrongSpringPtr pSpring)
{
    if(dt==0)
    {
        return ;
    }
    
    vec3 pos = m_pGeometry->GetVertexAtIndex(pSpring->GetParticleIndex1()).Position;
    vec3 pos_old = m_particles[pSpring->GetParticleIndex1()]->GetOldPosition();
    vec3 vel = (pos - pos_old) / dt;
    
    vec3 p2 = m_pGeometry->GetVertexAtIndex(pSpring->GetParticleIndex2()).Position;
    vec3 p2_last = m_particles[pSpring->GetParticleIndex2()]->GetOldPosition();
    
    vec3 v2 = (p2- p2_last)/dt;
    vec3 deltaP = pos - p2;
    vec3 deltaV = vel - v2;
    float dist = glm::length(deltaP);
    
    if(dist==0)
    {
        return ;
    }

    float leftTerm = -pSpring->GetKs() * (dist-pSpring->GetRestDistance());
    float rightTerm = pSpring->GetKd() * (glm::dot(deltaV, deltaP)/dist);
    
    vec3 springForce = (leftTerm + rightTerm) * normalize(deltaP);
    
    m_particles[pSpring->GetParticleIndex1()]->AddForce(springForce);
}

const vec3 ClothComponent::GetTotalForceOnCloth() const
{
    vec3 total;
    
    for(StrongClothParticlePtr pParticle : m_particles)
    {
        total += pParticle->GetAcceleration();
    }
    
    return total;
}

void ClothComponent::UpdateNormals()
{
	if (m_normalCalculationData.size() == 0)
		m_normalCalculationData.resize(m_numParticlesWidth * m_numParticlesHeight);
	std::fill(m_normalCalculationData.begin(), m_normalCalculationData.end(), vec3(0));

	auto& vertices = m_pGeometry->GetAllVertexData();

	//compute per face normals
	for (StrongTrianglePtr pTriangle : m_triangles)
	{
		vec3 normal = normalize(CalculateNormalOfTriangle(pTriangle));
		auto& particles = pTriangle->GetParticles();
		for (auto& p : particles)
			m_normalCalculationData[p->GetIndex()] += normal;
			//vertices[p->GetIndex()].Normal = normal;
	}

	//smooth normals using grid layout of vertices
	AABB box;
	for (size_t i = 0; i < vertices.size(); i++)
	{
		unsigned int x, y;
		GetParticleCoords(i, x, y);
		vec3 n = vec3(0.0f);
		if (x > 0)
			n += m_normalCalculationData[GetParticleIndex(x - 1, y)];
		if (x < m_numParticlesWidth - 1)
			n += m_normalCalculationData[GetParticleIndex(x + 1, y)];
		if (y > 0)
			n += m_normalCalculationData[GetParticleIndex(x, y - 1)];
		if (y < m_numParticlesHeight - 1)
			n += m_normalCalculationData[GetParticleIndex(x, y + 1)];
		vertices[i].Normal = normalize(n);
		box = box.Extend(vertices[i].Position);
	}

	m_pGeometry->SetLocalBox(box);
}

void ClothComponent::RemoveFixPoint(const glm::vec2 fixPoint)
{
    const unsigned int index =
                        GetParticleIndex(fixPoint.x, fixPoint.y);
    
    if(index>m_particles.size())
    {
        return ;
    }
    
    StrongClothParticlePtr pParticle = m_particles[index];
    
    pParticle->MakeMoveable();
}

void ClothComponent::TowPoint(
            const glm::vec2 gridPosition, const glm::vec3 towPoint, const float& t)
{
    int particleIndex =
                GetParticleIndex(gridPosition.x, gridPosition.y);
    
    if(m_particles[particleIndex]->IsMoveable())
    {
        return ;
    }
    
    const vec3 vertexPosition =
                m_pGeometry->GetVertexAtIndex(particleIndex).Position;
    const vec3 newPosition = towPoint + t * (vertexPosition - towPoint);
    
    //m_particles[particleIndex]->MakeMoveable();
    m_pGeometry->GetVertexAtIndex(particleIndex).Position = newPosition;
    
    //m_particles[particleIndex]->MakeUnmovable();
}
