
#ifndef JSON_TYPE_H
#define JSON_TYPE_H
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct ObjectMember;
struct ArrayValue;
struct JsonObject;

struct ObjectMember{
    char* key;
    JsonObject* value;
    ObjectMember* next;
};

struct ArrayValue{
    JsonObject* value;
    ArrayValue* next;
};

struct JsonObject{
    enum {OBJECT, ARRAY, STRING, NUMBER, BOOL, VNULL} category;
    
    union{
        ObjectMember* members;
        ArrayValue* values;
        char* string;
        double number;
        int boolean;
    };
};

void printValue(JsonObject* object, int indent);
void printArray(ArrayValue* arrayValue, int indent);
void printMember(ObjectMember* members, int indent);

bool checkValue(JsonObject* object);
bool checkArray(ArrayValue* arrayValue);
bool checkMember(ObjectMember* members);

void printMember(ObjectMember* members, int indent){
    if(members == NULL){
        return;
    }
    printf("%*c", indent*2, ' ');
    printf("member %s\n", members->key);
    printValue(members->value, indent);
    printMember(members->next, indent);
}
void printArray(ArrayValue* arrayValue, int indent){
    if(arrayValue == NULL){
        return;
    }
    
    printValue(arrayValue->value, indent);
    printArray(arrayValue->next, indent);
}

void printValue(JsonObject* object, int indent){
    printf("%*c", indent*2, ' ');
    
    if(object->category == JsonObject::OBJECT){
        
        printf("Member: \n");

        printMember(object->members, indent + 1);
    }
    else if(object->category == JsonObject::ARRAY){
        printf("Array: \n");
        printArray(object->values, indent + 1);
    }
    else if(object->category == JsonObject::STRING){
        printf("string: %s\n", object->string);
    }
    else if(object->category == JsonObject::NUMBER){
        printf("number: %lf\n", object->number);
    }
    else if(object->category == JsonObject::BOOL){
        printf("bool: %d\n", object->boolean);
    }
    else if(object->category == JsonObject::VNULL){
        printf("Null\n");
    }
}

bool checkMember(ObjectMember* members){
    if(members == NULL){
        return true;
    }
    return checkValue(members->value) && checkMember(members->next);
}
bool checkArray(ArrayValue* arrayValue){
    if(arrayValue == NULL){
        return true;
    }
    return checkValue(arrayValue->value) && checkArray(arrayValue->next);
}

bool checkValue(JsonObject* object){
    if(object->category == JsonObject::OBJECT){
        unordered_set<char> temp;
        auto pt = object->members;
        while(pt != NULL){
            if(temp.find(pt->key[1]) == temp.end()){
                temp.insert(pt->key[1]);
            }
            else{
                printf("duplicate key: %s\n", pt->key);
                return false;
            }
            pt = pt->next;
        }
        return checkMember(object->members);
    }
    else if(object->category == JsonObject::ARRAY){
        return checkArray(object->values);
    }
    return true;
}
JsonObject* NewType(){
    JsonObject* type =  (JsonObject*)malloc(sizeof(JsonObject));
    return type;
}
//string, number, bool, null
JsonObject* NewString(char* str){
     JsonObject* type =  (JsonObject*)malloc(sizeof(JsonObject));
    type->category = JsonObject::STRING;
    type->string = (char*)malloc((strlen(str)+ 1)* sizeof(char));
    strcpy(type->string, str);
    return type;
}
JsonObject* NewNumber(char* num_char){
    double num = atof(num_char);
    JsonObject* type =  (JsonObject*)malloc(sizeof(JsonObject));
    type->category = JsonObject::NUMBER;
    type->number = num;
    return type;
}
JsonObject* NewBoolean(int boolean){
    JsonObject* type =  (JsonObject*)malloc(sizeof(JsonObject));
    type->category = JsonObject::BOOL;
    type->boolean = boolean;
    return type;
}
JsonObject* NewNull(){
    JsonObject* type =  (JsonObject*)malloc(sizeof(JsonObject));
    type->category = JsonObject::VNULL;
    return type;
}

JsonObject* NewObject(const char* key, JsonObject* object){
    JsonObject* type =  (JsonObject*)malloc(sizeof(JsonObject));
    type->category = JsonObject::OBJECT;
    type->members = (ObjectMember*)malloc(sizeof(ObjectMember));
    type->members->key = (char*)malloc(sizeof(char) * strlen(key));
    strcpy(type->members->key, key);
    type->members->value = object;
    type->members->next = NULL;
    return type;
}
JsonObject* NewArray(JsonObject* object){
    JsonObject* type = (JsonObject*)malloc(sizeof(JsonObject));
    type->category = JsonObject::ARRAY;
    type->values = (ArrayValue*)malloc(sizeof(ArrayValue));
    type->values->value = object;
    type->values->next = NULL;
    return type;
}
ObjectMember* NewMember(JsonObject* string, JsonObject* object){
    ObjectMember*member =  (ObjectMember*)malloc(sizeof(ObjectMember));
    member->key = (char*) malloc(sizeof(char) * strlen(string->string));
    strcpy(member->key, string->string);
    member->value = object;
    member->next = NULL;
    return member;
}
JsonObject* AddArrayItem(JsonObject* object, JsonObject* objectList){
    ArrayValue* temp = (ArrayValue*)malloc(sizeof(ArrayValue));
    temp->value = object;
    temp->next = objectList->values;
    objectList->values = temp;
    return objectList;
}
ObjectMember* AddMemberItem(ObjectMember* member, ObjectMember* members){
    member->next = members;
    return member;
}
JsonObject* NewObjectFromMember(ObjectMember* member){
    JsonObject* type = (JsonObject*)malloc(sizeof(JsonObject));
    type->category = JsonObject::OBJECT;
    type->members = member;
    return type;
}
#endif