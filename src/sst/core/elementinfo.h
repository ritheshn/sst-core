// Copyright 2009-2016 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
// 
// Copyright (c) 2009-2016, Sandia Corporation
// All rights reserved.
// 
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#ifndef SST_CORE_ELEMENTINFO_H
#define SST_CORE_ELEMENTINFO_H

#include <sst/core/sst_types.h>
#include <sst/core/params.h>

#include <stdio.h>
#include <string>

#include <sst/core/elibase.h>

// // Component Category Definitions
// #define COMPONENT_CATEGORY_UNCATEGORIZED  0x00
// #define COMPONENT_CATEGORY_PROCESSOR      0x01
// #define COMPONENT_CATEGORY_MEMORY         0x02
// #define COMPONENT_CATEGORY_NETWORK        0x04
// #define COMPONENT_CATEGORY_SYSTEM         0x08

namespace SST {
class Component;
class Module;
class SubComponent;
namespace Partition {
    class SSTPartitioner;
}
class RankInfo;


/****************************************************
   Base classes for templated documentation classes
*****************************************************/

class BaseComponentElementInfo {

protected:
    std::vector<std::string> portnames;
    Params::KeySet_t allowedKeys;
    std::vector<std::string> statnames;
    
    void initialize();
    
public:

    // virtual Component* create(ComponentId_t id, Params& params) = 0;
    virtual const std::string getDescription() = 0;
    virtual const std::string getName() = 0;
    virtual const std::string getLibrary() = 0;
    virtual const std::vector<ElementInfoParam>& getValidParams() = 0;
    virtual const std::vector<ElementInfoStatistic>& getValidStats() = 0;
    virtual const std::vector<ElementInfoPort2>& getValidPorts() = 0;

    const std::vector<std::string>& getPortnames() { return portnames; }
    const std::vector<std::string>& getStatnames() { return statnames; }
    const Params::KeySet_t& getParamNames() { return allowedKeys; }

    std::string getParametersString();
    std::string getStatisticsString();
    std::string getPortsString();
    
};


class ComponentElementInfo : public BaseComponentElementInfo {

public:

    ComponentElementInfo() {  }
    
    virtual Component* create(ComponentId_t id, Params& params) = 0;
    virtual const uint32_t getCategory() = 0;
    
    void print() {
        std::cout << "    " << getName() << ": " << getDescription() << std::endl;
        std::cout << "    Parameters (" << getValidParams().size() << " total):"<<  std::endl;
        for ( auto item : getValidParams() ) {
            std::cout << "      " << item.name << ": "
                      << (item.description == NULL ? "<empty>" : item.description)
                      << " ("
                      << (item.defaultValue == NULL ? "<required>" : item.defaultValue)
                      << ")" << std::endl;
        }
        std::cout << "    Statistics (" << getValidStats().size() << " total):"<<  std::endl;
        for ( auto item : getValidStats() ) {
            std::cout << "      " << item.name << ": "
                      << (item.description == NULL ? "<empty>" : item.description)
                      << " ("
                      << (item.units == NULL ? "<empty>" : item.units)
                      << ").  Enable level = " << (int16_t)item.enableLevel << std::endl;
        }
        std::cout << "    Ports (" << getValidPorts().size() << " total):"<<  std::endl;
        for ( auto item : getValidPorts() ) {
            std::cout << "      " << item.name << ": "
                      << (item.description == NULL ? "<empty>" : item.description)
                      << std::endl;
        }
    }
};

class SubComponentElementInfo : public BaseComponentElementInfo {

public:

    virtual SubComponent* create(Component* comp, Params& params) = 0;
    // virtual SubComponent* create(ComponentId_t id, Params& params) = 0;
    virtual const std::string getInterface() = 0;

    void print() {
        std::cout << "    " << getName() << ": " << getDescription() << std::endl;
        std::cout << "    Parameters (" << getValidParams().size() << " total):"<<  std::endl;
        for ( auto item : getValidParams() ) {
            std::cout << "      " << item.name << ": "
                      << (item.description == NULL ? "<empty>" : item.description)
                      << " ("
                      << (item.defaultValue == NULL ? "<required>" : item.defaultValue)
                      << ")" << std::endl;
        }
        std::cout << "    Statistics (" << getValidStats().size() << " total):"<<  std::endl;
        for ( auto item : getValidStats() ) {
            std::cout << "      " << item.name << ": "
                      << (item.description == NULL ? "<empty>" : item.description)
                      << " ("
                      << (item.units == NULL ? "<empty>" : item.units)
                      << ").  Enable level = " << (int16_t)item.enableLevel << std::endl;
        }
        // std::cout << "    Ports (" << getValidPorts().size() << " total):"<<  std::endl;
        // for ( auto item : getValidPorts() ) {
        //     std::cout << "      " << item.name << ": "
        //               << (item.description == NULL ? "<empty>" : item.description)
        //               << std::endl;
        // }
    }
};

class ModuleElementInfo {

public:
    virtual Module* create() = 0;
};

 // class PythonModuleInfo {
 //     virtual const std::string getFoo() = 0;

 //     std::string 
     
 // };
 

class LibraryInfo {
public:
    std::map<std::string,ComponentElementInfo*> components;
    std::map<std::string,SubComponentElementInfo*> subcomponents;
    // std::vector<PythonModuleElementInfo*> python_modules;
    
    ComponentElementInfo* getComponent(std::string name) {
        if ( components.count(name) == 0 ) return NULL;
        return components[name];
    }
    
    SubComponentElementInfo* getSubComponent(std::string name) {
        if ( subcomponents.count(name) == 0 ) return NULL;
        return subcomponents[name];
    }
    
    void print() {
        std::cout << "  Components: " << std::endl;
        for ( auto item : components ) {
            item.second->print();
            std::cout << std::endl;
        }
        std::cout << "  SubComponents: " << std::endl;
        for ( auto item : subcomponents ) {
            item.second->print();
            std::cout << std::endl;
        }
    }
};


class ElementLibraryDatabase {
private:
    // Database
    static std::map<std::string,LibraryInfo*> libraries;

    static LibraryInfo* getLibrary(std::string library) {
        if ( libraries.count(library) == 0 ) {
            libraries[library] = new LibraryInfo;
        }
        return libraries[library];
    }
    
public:
    static bool addComponent(ComponentElementInfo* comp) {
        LibraryInfo* library = getLibrary(comp->getLibrary());
        library->components[comp->getName()] = comp;
        return true;
    }

    static bool addSubComponent(SubComponentElementInfo* comp) {
        LibraryInfo* library = getLibrary(comp->getLibrary());
        // library->subcomponents.push_back(comp);
        library->subcomponents[comp->getName()] = comp;
        return true;
    }

    static void printDatabase() {
        for ( auto item : libraries ) {
            std::cout << "library : " << item.first << std::endl;
            item.second->print();
        }
    }

    static LibraryInfo* getLibraryInfo(std::string library) {
        if ( libraries.count(library) == 0 ) return NULL;
        return libraries[library];
    }
    
    // static Component* createComponent(std::string lib, std::string name, ComponentId_t id, Params& params) {
        
    // }
    
};



/**************************************************************************
  Templated classes that are used to actually implement the element info
  documenation in the database.
**************************************************************************/


/**************************************************************************
  Classes to support Components
**************************************************************************/


template <class T>
class ComponentDoc : public ComponentElementInfo {
private:
    static const bool loaded;
    
public:

    ComponentDoc() : ComponentElementInfo() {
        initialize();
    }
    
    Component* create(ComponentId_t id, Params& params) {
        return new T(id, params);
    }

    static const bool isLoaded() { return loaded; }
    const std::string getLibrary() { return T::ELI_getLibrary(); }
    const std::string getName() { return T::ELI_getName(); }
    const std::string getDescription() { return T::ELI_getDescription(); }
    const std::vector<ElementInfoParam>& getValidParams() { return T::ELI_getParams(); }
    const std::vector<ElementInfoStatistic>& getValidStats() { return T::ELI_getStatistics(); }
    const std::vector<ElementInfoPort2>& getValidPorts() { return T::ELI_getPorts(); }
    const uint32_t getCategory() { return T::ELI_getCategory(); };
};


// This version gets instanced if there is a custom create function
template <class T>
class ComponentDocCustomCreate : public ComponentDoc<T> {
public:
    ComponentDocCustomCreate() : ComponentDoc<T>() {
    }
    
    Component* create(ComponentId_t id, Params& params) {
        return T::ELI_Custom_Create(id, params);
    }
};

// Class to check to see if the function custom_create exists.
template <class T>
class checkForCustomCreateComponent
{
    template <typename F, F> struct check;

    typedef char Match;
    typedef long NotMatch;

    // Used for non-static
    // template <typename F>
    // struct FunctionSignature
    // {
    //     typedef Component* (F::*function)(ComponentId_t,Params&);
    // };
        
    struct FunctionSignature
    {
        typedef Component* (*function)(ComponentId_t,Params&);
    };

    // Used for non-static functions
    // template <typename F> static Match HasCustomCreate(check< typename FunctionSignature<F>::function, &F::ELI_Custom_Create >*);
    template <typename F> static Match HasCustomCreate(check< typename FunctionSignature::function, &F::ELI_Custom_Create >*);
    template <typename F> static NotMatch HasCustomCreate(...);

public:
    static bool const value = (sizeof(HasCustomCreate<T>(0)) == sizeof(Match) );
};

// These static functions choose between the custom and not custom
// create versions by looking for ELI_Custom_Create
template<class T>
typename std::enable_if<checkForCustomCreateComponent<T>::value, ComponentElementInfo*>::type
createComponentDoc() {
    return new ComponentDocCustomCreate<T>();
}

template<class T>
typename std::enable_if<not checkForCustomCreateComponent<T>::value, ComponentElementInfo*>::type
createComponentDoc() {
    return new ComponentDoc<T>();
}


// template<class T> const bool ComponentDoc<T>::loaded = ElementLibraryDatabase::addComponent(new ComponentDoc<T>());
template<class T> const bool ComponentDoc<T>::loaded = ElementLibraryDatabase::addComponent(createComponentDoc<T>());


/**************************************************************************
  Classes to support SubComponents
**************************************************************************/

template <class T>
class SubComponentDoc : public SubComponentElementInfo {
private:
    static const bool loaded;

public:
    
    SubComponentDoc() : SubComponentElementInfo() {
        initialize();
    }

    SubComponent* create(Component* comp, Params& params) {
        return new T(comp,params);
    }

    static const bool isLoaded() { return loaded; }
    const std::string getLibrary() { return T::ELI_getLibrary(); }
    const std::string getName() { return T::ELI_getName(); }
    const std::string getDescription() { return T::ELI_getDescription(); }
    const std::vector<ElementInfoParam>& getValidParams() { return T::ELI_getParams(); }
    const std::vector<ElementInfoStatistic>& getValidStats() { return T::ELI_getStatistics(); }
    const std::vector<ElementInfoPort2>& getValidPorts() { return T::ELI_getPorts(); }
    const std::string getInterface() { return T::ELI_getInterface(); }

};

// This version gets instanced if there is a custom create function
template <class T>
class SubComponentDocCustomCreate : public SubComponentDoc<T> {
public:
    SubComponentDocCustomCreate() : SubComponentDoc<T>() {
    }
    
    SubComponent* create(Component* comp, Params& params) {
        return T::ELI_Custom_Create(comp, params);
    }
};

// Class to check to see if the function custom_create exists.
template <class T>
class checkForCustomCreateSubComponent
{
    template <typename F, F> struct check;

    typedef char Match;
    typedef long NotMatch;

    struct FunctionSignature
    {
        typedef SubComponent* (*function)(Component*,Params&);
    };

    // Used for non-static functions
    // template <typename F> static Match HasCustomCreate(check< typename FunctionSignature<F>::function, &F::ELI_Custom_Create >*);
    template <typename F> static Match HasCustomCreate(check< typename FunctionSignature::function, &F::ELI_Custom_Create >*);
    template <typename F> static NotMatch HasCustomCreate(...);

public:
    static bool const value = (sizeof(HasCustomCreate<T>(0)) == sizeof(Match) );
};

// These static functions choose between the custom and not custom
// create versions by looking for ELI_Custom_Create
template<class T>
typename std::enable_if<checkForCustomCreateComponent<T>::value, SubComponentElementInfo*>::type
createSubComponentDoc() {
    return new SubComponentDocCustomCreate<T>();
}

template<class T>
typename std::enable_if<not checkForCustomCreateSubComponent<T>::value, SubComponentElementInfo*>::type
createSubComponentDoc() {
    return new SubComponentDoc<T>();
}


// template<class T> const bool SubComponentDoc<T>::loaded = ElementLibraryDatabase::addSubComponent(new SubComponentDoc<T>());
template<class T> const bool SubComponentDoc<T>::loaded = ElementLibraryDatabase::addSubComponent(createSubComponentDoc<T>());


/**************************************************************************
  Classes to support Modules
**************************************************************************/



#define SST_ELI_REGISTER_COMPONENT(cls,lib,name,desc,cat)  \
    friend class ComponentDoc<cls>; \
    bool ELI_isLoaded() {                           \
        return ComponentDoc<cls>::isLoaded();     \
    } \
    static const std::string ELI_getLibrary() { \
      return lib; \
    } \
    static const std::string ELI_getName() { \
      return name; \
    } \
    static const std::string ELI_getDescription() {  \
      return desc; \
    } \
    static const uint32_t ELI_getCategory() {  \
      return cat; \
    }

#define SST_ELI_DOCUMENT_PARAMS(...) \
    static const std::vector<ElementInfoParam>& ELI_getParams() { \
        static std::vector<ElementInfoParam> var = { __VA_ARGS__ } ; \
        return var; \
    }

// #define DOCUMENT_STATISTICS(...) \
//     static const std::vector<ElementInfoStatistic>& ELI_getStatistics() {  \
// return std::vector<ElementInfoStatistic> { __VA_ARGS__ } ;      \
//     }

#define SST_ELI_DOCUMENT_STATISTICS(...) \
    static const std::vector<ElementInfoStatistic>& ELI_getStatistics() {  \
        static std::vector<ElementInfoStatistic> var = { __VA_ARGS__ } ;  \
        return var; \
    }

#define SST_ELI_DOCUMENT_PORTS(...) \
    static const std::vector<ElementInfoPort2>& ELI_getPorts() { \
        static std::vector<ElementInfoPort2> var = { __VA_ARGS__ } ;      \
        return var; \
    }



#define SST_ELI_REGISTER_SUBCOMPONENT(cls,lib,name,desc,interface)   \
    friend class SubComponentDoc<cls>; \
    bool ELI_isLoaded() {                           \
      return SubComponentDoc<cls>::isLoaded(); \
    } \
    static const std::string ELI_getLibrary() { \
      return lib; \
    } \
    static const std::string ELI_getName() { \
      return name; \
    } \
    static const std::string ELI_getDescription() {  \
      return desc; \
    } \
    static const std::string ELI_getInterface() {  \
      return interface; \
    }


} //namespace SST

#endif // SST_CORE_ELEMENTINFO_H
