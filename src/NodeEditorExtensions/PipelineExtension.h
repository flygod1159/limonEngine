//
// Created by engin on 15.04.2019.
//

#ifndef LIMONENGINE_PIPELINEEXTENSION_H
#define LIMONENGINE_PIPELINEEXTENSION_H


#include <nodeGraph/src/EditorExtension.h>
#include "API/Graphics/GraphicsInterface.h"
#include "Graphics/GraphicsPipeline.h"

#include <functional>
#include <vector>
#include <unordered_set>

class GraphicsPipelineStage;

class PipelineExtension : public EditorExtension {
    std::map<std::string, std::shared_ptr<Texture>> usedTextures;
    GraphicsInterface* graphicsWrapper = nullptr;
    std::shared_ptr<AssetManager> assetManager; //TODO: used for deserialize textures, maybe it would be possible to avoid.
    Options* options;//TODO: used for texture de/serialize, maybe it would be possible to avoid.
    const std::vector<std::string>& renderMethodNames;
    std::vector<std::string> messages;
    std::vector<std::string> errorMessages;
    RenderMethods renderMethods;

    std::shared_ptr<GraphicsPipeline> builtPipeline = nullptr;
    bool nodeGraphValid = true; //if there are nodes that are unknown, then we can't build.

    static bool getNameOfTexture(void* data, int index, const char** outText);

    bool buildRenderPipelineRecursive(const Node *node, std::shared_ptr<GraphicsPipeline> graphicsPipeline, std::map<const Node*, std::shared_ptr<GraphicsPipeline::StageInfo>>& nodeStages,
                                      const std::vector<std::pair<std::set<const Node*>, std::set<const Node*>>>& groupsByDependency,
                                      std::vector<std::shared_ptr<GraphicsPipeline::StageInfo>>& builtStages);
    void buildDependencyInfoRecursive(const Node *node, std::unordered_map<const Node*, std::set<const Node*>>& dependencies);
    std::vector<std::pair<std::set<const Node*>, std::set<const Node*>>> buildGroupsByDependency(std::unordered_map<const Node*, std::set<const Node*>>);
    bool canBeJoined(const std::set<const Node*>& existingNodes, const std::set<const Node*>& existingDependencies, const Node* currentNode, const std::set<const Node*>& currentDependencies);

    static std::shared_ptr<GraphicsPipeline::StageInfo>
    findSharedStage(const Node *currentNode, std::map<const Node *, std::shared_ptr<GraphicsPipeline::StageInfo>> &builtStages,
                    const std::vector<std::pair<std::set<const Node *>, std::set<const Node *>>> &dependencyGroups);

public:
    PipelineExtension(GraphicsInterface *graphicsWrapper, std::shared_ptr<GraphicsPipeline> currentGraphicsPipeline, std::shared_ptr<AssetManager> assetManager, Options* options,
                      const std::vector<std::string> &renderMethodNames, RenderMethods renderMethods);

    void drawDetailPane(NodeGraph* nodeGraph, const std::vector<const Node *>& nodes, const Node* selectedNode) override;

    const std::map<std::string, std::shared_ptr<Texture>> &getUsedTextures() const {
        return usedTextures;
    }

    std::string getName() override {
        return "PipelineExtension";
    }

    void serialize(tinyxml2::XMLDocument &document, tinyxml2::XMLElement *parentElement) override;

    void deserialize(const std::string &fileName, tinyxml2::XMLElement *editorExtensionElement) override;

    std::shared_ptr<GraphicsPipeline> buildRenderPipeline(const std::vector<const Node *> &nodes);


    void addError(const std::string& errorMessage) {
        this->errorMessages.emplace_back(errorMessage);
    }

    void addMessage(const std::string& message) {
        this->messages.emplace_back(message);
    }

    bool isPipelineBuilt() {
        return builtPipeline != nullptr;
    }

    std::shared_ptr<GraphicsPipeline> handOverBuiltPipeline() {
        std::shared_ptr<GraphicsPipeline> temp = builtPipeline;
        builtPipeline = nullptr;
        return temp;
    }

    bool isNodeGraphValid() const {
        return nodeGraphValid;
    }

    void setNodeGraphValid(bool nodeGraphValid) {
        PipelineExtension::nodeGraphValid = nodeGraphValid;
        if(!nodeGraphValid) {
            addError("Node Graph is not valid, can't build pipeline!");
        }
    }

};


#endif //LIMONENGINE_PIPELINEEXTENSION_H
