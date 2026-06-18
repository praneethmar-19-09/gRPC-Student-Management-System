#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "student.grpc.pb.h"
#include "student.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using student::Student;
using student::StudentList;
using student::StudentService;

using student::AddStudentRequest;
using student::DeleteStudentRequest;
using student::GetStudentRequest;
using student::UpdateStudentRequest;

using student::Empty;
using student::OperationStatus;

class StudentClient {
public:
  StudentClient(std::shared_ptr<Channel> channel)
      : stub_(StudentService::NewStub(channel)) {}

  void AddStudent() {
    AddStudentRequest request;

    int id;
    int age;

    std::string name;
    std::string department;

    std::cout << "ID: ";
    std::cin >> id;

    std::cout << "Name: ";
    std::cin >> name;

    std::cout << "Age: ";
    std::cin >> age;

    std::cout << "Department: ";
    std::cin >> department;

    request.set_id(id);
    request.set_name(name);
    request.set_age(age);
    request.set_department(department);

    OperationStatus response;

    ClientContext context;

    Status status = stub_->AddStudent(&context, request, &response);

    if (status.ok()) {
      std::cout << response.message() << std::endl;
    } else {
      std::cout << "RPC Failed: " << status.error_message() << std::endl;
    }
  }

  void GetStudent() {
    GetStudentRequest request;

    int id;

    std::cout << "Student ID: ";
    std::cin >> id;

    request.set_id(id);

    Student response;

    ClientContext context;

    Status status = stub_->GetStudent(&context, request, &response);

    if (status.ok()) {
      PrintStudent(response);
    } else {
      std::cout << "Error: " << status.error_message() << std::endl;
    }
  }

  void ListStudents() {
    Empty request;

    StudentList response;

    ClientContext context;

    Status status = stub_->ListStudents(&context, request, &response);

    if (!status.ok()) {
      std::cout << "RPC Failed: " << status.error_message() << std::endl;

      return;
    }

    std::cout << "\nTotal Students: " << response.students_size() << "\n"
              << std::endl;

    for (const auto &student : response.students()) {
      PrintStudent(student);
      std::cout << std::endl;
    }
  }

  void UpdateStudent() {
    UpdateStudentRequest request;

    int id;
    int age;

    std::string name;
    std::string department;

    std::cout << "ID: ";
    std::cin >> id;

    std::cout << "New Name: ";
    std::cin >> name;

    std::cout << "New Age: ";
    std::cin >> age;

    std::cout << "New Department: ";
    std::cin >> department;

    request.set_id(id);
    request.set_name(name);
    request.set_age(age);
    request.set_department(department);

    OperationStatus response;

    ClientContext context;

    Status status = stub_->UpdateStudent(&context, request, &response);

    if (status.ok()) {
      std::cout << response.message() << std::endl;
    } else {
      std::cout << status.error_message() << std::endl;
    }
  }

  void DeleteStudent() {
    DeleteStudentRequest request;

    int id;

    std::cout << "Student ID: ";
    std::cin >> id;

    request.set_id(id);

    OperationStatus response;

    ClientContext context;

    Status status = stub_->DeleteStudent(&context, request, &response);

    if (status.ok()) {
      std::cout << response.message() << std::endl;
    } else {
      std::cout << status.error_message() << std::endl;
    }
  }

private:
  void PrintStudent(const Student &student) {
    std::cout << "ID         : " << student.id() << std::endl;

    std::cout << "Name       : " << student.name() << std::endl;

    std::cout << "Age        : " << student.age() << std::endl;

    std::cout << "Department : " << student.department() << std::endl;
  }

private:
  std::unique_ptr<StudentService::Stub> stub_;
};

int main() {
  StudentClient client(grpc::CreateChannel("localhost:50051",
                                           grpc::InsecureChannelCredentials()));

  while (true) {
    int choice;

    std::cout << "\n";
    std::cout << "===== Student Menu =====\n";
    std::cout << "1. Add Student\n";
    std::cout << "2. Get Student\n";
    std::cout << "3. List Students\n";
    std::cout << "4. Update Student\n";
    std::cout << "5. Delete Student\n";
    std::cout << "6. Exit\n";
    std::cout << "Choice: ";

    std::cin >> choice;

    switch (choice) {
    case 1:
      client.AddStudent();
      break;

    case 2:
      client.GetStudent();
      break;

    case 3:
      client.ListStudents();
      break;

    case 4:
      client.UpdateStudent();
      break;

    case 5:
      client.DeleteStudent();
      break;

    case 6:
      return 0;

    default:
      std::cout << "Invalid choice" << std::endl;
    }
  }

  return 0;
}