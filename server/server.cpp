#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <grpcpp/grpcpp.h>

#include "student.pb.h"
#include "student.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using student::Student;
using student::StudentList;
using student::StudentService;

using student::AddStudentRequest;
using student::GetStudentRequest;
using student::DeleteStudentRequest;
using student::UpdateStudentRequest;

using student::OperationStatus;
using student::Empty;

class StudentServiceImpl final : public StudentService::Service
{
private:
    std::unordered_map<int, Student> students_;

public:

    Status AddStudent(
        ServerContext* context,
        const AddStudentRequest* request,
        OperationStatus* response) override
    {
        (void)context;

        if (students_.find(request->id()) != students_.end())
        {
            response->set_success(false);
            response->set_message("Student already exists");

            return Status::OK;
        }

        Student student;

        student.set_id(request->id());
        student.set_name(request->name());
        student.set_age(request->age());
        student.set_department(request->department());

        students_[student.id()] = student;

        response->set_success(true);
        response->set_message("Student added successfully");

        std::cout
            << "[ADD] Student ID="
            << student.id()
            << " Name="
            << student.name()
            << std::endl;

        return Status::OK;
    }

    Status GetStudent(
        ServerContext* context,
        const GetStudentRequest* request,
        Student* response) override
    {
        (void)context;

        auto it = students_.find(request->id());

        if (it == students_.end())
        {
            return Status(
                grpc::StatusCode::NOT_FOUND,
                "Student not found");
        }

        *response = it->second;

        std::cout
            << "[GET] Student ID="
            << request->id()
            << std::endl;

        return Status::OK;
    }

    Status ListStudents(
        ServerContext* context,
        const Empty* request,
        StudentList* response) override
    {
        (void)context;
        (void)request;

        for (const auto& [id, student] : students_)
        {
            Student* newStudent =
                response->add_students();

            *newStudent = student;
        }

        std::cout
            << "[LIST] Total Students="
            << students_.size()
            << std::endl;

        return Status::OK;
    }

    Status UpdateStudent(
        ServerContext* context,
        const UpdateStudentRequest* request,
        OperationStatus* response) override
    {
        (void)context;

        auto it = students_.find(request->id());

        if (it == students_.end())
        {
            response->set_success(false);
            response->set_message("Student not found");

            return Status::OK;
        }

        it->second.set_name(request->name());
        it->second.set_age(request->age());
        it->second.set_department(request->department());

        response->set_success(true);
        response->set_message("Student updated successfully");

        std::cout
            << "[UPDATE] Student ID="
            << request->id()
            << std::endl;

        return Status::OK;
    }

    Status DeleteStudent(
        ServerContext* context,
        const DeleteStudentRequest* request,
        OperationStatus* response) override
    {
        (void)context;

        auto it = students_.find(request->id());

        if (it == students_.end())
        {
            response->set_success(false);
            response->set_message("Student not found");

            return Status::OK;
        }

        students_.erase(it);

        response->set_success(true);
        response->set_message("Student deleted successfully");

        std::cout
            << "[DELETE] Student ID="
            << request->id()
            << std::endl;

        return Status::OK;
    }
};

void RunServer()
{
    const std::string server_address =
        "0.0.0.0:50051";

    StudentServiceImpl service;

    ServerBuilder builder;

    builder.AddListeningPort(
        server_address,
        grpc::InsecureServerCredentials());

    builder.RegisterService(&service);

    std::unique_ptr<Server> server(
        builder.BuildAndStart());

    std::cout
        << "Student gRPC Server listening on "
        << server_address
        << std::endl;

    server->Wait();
}

int main()
{
    RunServer();
    return 0;
}