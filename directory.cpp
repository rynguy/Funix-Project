// Authors: Armante Washington & Ryan Nguyen
// directory.cpp
//NOTES: Just the directory segment of the entire pseudo Unix program
#include "directory.h"
#include "Time.h"
#include "permissions.h"
#include "file.h"
#include <cstring>

using namespace std;

Directory::Directory(const char *nam, Directory *paren, const char *owner)
  : File(nam, permissions = (Permissions(01777, owner))), subDirectoryCount(0),
  permissions(Permissions(01777, owner)), parent(paren)
{
  name = new char[strlen(nam) + 1];
  strcpy(name, nam);
} // Directory()


Directory::Directory(const Directory &rhs) : File(rhs), subDirectoryCount(rhs.subDirectoryCount), 
   permissions(rhs.permissions),  parent(rhs.parent)
{
  name = new char[strlen(rhs.name) + 1];
  strcpy(name, rhs.name);

  for(int i = 0; i < subDirectoryCount; i++)
  {
    if (rhs.subDirectories[i]->isFile())
    {
      subDirectories += new File(*((File *)rhs.subDirectories[i]));
    }
    else
    {
      Directory *temp = new Directory(*((Directory*) rhs.subDirectories[i]));
      temp->parent = this;
      subDirectories += temp;
    }
  }  // for each subdirectory
} // Directory copy constructor


/*Directory::~Directory()
{
  delete [] name;
}*/ // ~Directory()


Directory* Directory::cd(int argCount, const char *arguments[], const char *user)
{
  if(argCount != 2)
  {
    cout << "usage: cd directoryName\n";
    return this;
  } // if incorrect number of arguments

  if(strcmp(arguments[1], "..") == 0)
  {
    if(parent)
    {
      if(! parent->permissions.check(5, user))
      {
        cout << "cd: Permission denied.\n";
        return this;
      } // if incorrect permissions
      return parent;
    }
    else // root directory
      return this;
  } // if cd ..

  for(int i = 0; i < subDirectoryCount; i++)
    if(Directory(arguments[1]) == *subDirectories[i])
    {
      if(subDirectories[i]->isFile())
      {
        cout << "cd: Permission denied.\n";
        return this;
      }//if it is a file
      if(!((Directory*)subDirectories[i])->permissions.check(5, user))
      {
        cout << "cd: Permission denied.\n";
        return this;
      } // if incorrect permissions for this subdirectory

      return ((Directory*)subDirectories[i]);
    } // if found subdirectory match

  cout << arguments[1] << ": no such file or directory.\n";
  return this;
} // cd()

void Directory::touch(int argCount, const char* arguments[], const char* user)
{
  int i;
  if(argCount < 2)
  {
    cout << "touch: too few arguments\n";
    return;
  } // if check arguments
  if(! permissions.check(6, user))
  {
    cout << "touch: Permission denied.\n";
    return;
  } // if don't have permissions
  
  for(int j = 1; j < argCount; j++) 
  {
    for(i = 0; i < subDirectoryCount; i++)
    {
      if (subDirectories[i]->isFile())
      {
        if(strcmp(arguments[j], subDirectories[i]->getName()) == 0)
        {
          subDirectories[i]->touch();
          break;
        } // if found file with same name
      }//if file is a file type
      else
      {
        if(strcmp(arguments[j], ((Directory*)subDirectories[i])->name) == 0)
        {
          ((Directory*)subDirectories[i])->modificationTime.update();
          break;
        } // if found file with same name
      } //if file is a directory type
    }
    if(i == subDirectoryCount)
    {
      Permissions *perm = new Permissions(00666, user);
      File *temp = new File(arguments[j], *perm);
      subDirectories += temp;
      subDirectoryCount++;
    }//if file doesn't exist
  }// for each argument
}// for each subdirectory[i]

void Directory::chmod(int argCount, const char *arguments[], const char *user)
{
  int i, j, value = 0;

  if(argCount < 3)
  {
    cout << "chmod: too few arguments\n";
    return;
  } // if incorrect argument count.

  for(i = 0; arguments[1][i]; i++)
    if(! isdigit(arguments[1][i]) || arguments[1][i] == '9'
      || arguments[1][i] == '8')
    {
      cout << "chmod: invalid mode string: '" << arguments[1] << "'\n";
      return;
    } // if invalid mode
    else // valid digit
      value = 8 * value + arguments[1][i] - '0';

  if(value < 0 || value > 0777)
  {
    cout << "chmod: invalid mode string: '" << arguments[1] << "'\n";
    return;
  } // if invalid value

  for(i = 2; i < argCount; i++)
  {
    for(j = 0; j < subDirectoryCount; j++)
      if(*subDirectories[j] == Directory(arguments[i]))
      {
        if(subDirectories[j]->isFile())
        {
          if(strcmp(user, "root") && strcmp(user, 
            ((File*)subDirectories[j])->permission->getOwner()) != 0
            && !((File*)subDirectories[j])->checkPerms(2, user))
          {
            cout << "chmod: Permission denied.\n";
            return;
          } // if invalid permissions for file

          ((File*)subDirectories[j])->changePerms(value);
          break;
        }// if file is is just a file
        else
        {
          if(strcmp(user, "root") && strcmp(user,
            ((Directory*)subDirectories[j])->permissions.getOwner()) != 0 
            && ! ((Directory*)subDirectories[j])->permissions.check(2, user))
          {
            cout << "chmod: Permission denied.\n";
            return;
          } // if invalid permissions for subdirectory

          ((Directory*)subDirectories[j])->permissions.chmodDos(value);
          break;
        }//else it is a directory type file
      } // if found subdirectory match

    if(j == subDirectoryCount)
      cout << "chmod: failed to get attributes of '" << arguments[i]
        << "': No such file or directory\n";
  } // for each file
} // chmod()


void Directory::chown( int argCount, const char *arguments[], const char *user)
{
  int i, j;

  if(argCount < 3)
  {
    cout << "chown: too few arguments\n";
    return;
  } // if incorrect argument count.


  for(i = 2; i < argCount; i++)
  {
    for(j = 0; j < subDirectoryCount; j++)
      if(*subDirectories[j] == Directory(arguments[i]))
      {
        if(strcmp("root", user))
        {
          cout << "chown: Permission denied.\n";
          return;
        } // if invalid permissions for file
        if(subDirectories[j]->isFile())
        {
          subDirectories[j]->chown(arguments[1]);
          break;
        } //if is a file
        else
        {
          ((Directory*)subDirectories[j])->permissions.setOwner(arguments[1]);
          break;
        }//else is directory
      } // if found file match

    if(j == subDirectoryCount)
      cout << "chown: failed to get attributes of '" << arguments[i]
        << "': No such file or directory\n";
  } // for each file in arguments

} // chown()


void Directory::cp( int argCount, const char *arguments[], const char *user)
{
  int i;

  if(argCount == 1)
  {
    cout << "cp: missing file arguments\n";
    return;
  } // if incorrect argument count.

  if(argCount == 2)
  {
    cout << "cp: missing destination file\n";
    return;
  } // if incorrect argument count.

  if(argCount > 3)
  {
    cout << "cp: too many arguments";
    return;
  } // if incorrect argument count.

  for(i = 0; i < subDirectoryCount; i++)
    if(*subDirectories[i] == Directory(arguments[1]))
      break;
  
  if(i == subDirectoryCount)
  {
    cout << "cp: cannot stat '" << arguments[1]
      << "': No such file or directory\n";
    return;
  } // if didn't find file

  if(!strcmp(arguments[1], arguments[2]))
  {
    cout << "cp: '" << arguments[1] << "' and '" << arguments[2]
      << "' are the same file\n";
    return;
  } // if same names
  
  if(subDirectories[i]->isFile())
  {
    if((subDirectories[i]->checkPerms(4, user)))
    {
      File *temp = new File(*subDirectories[i]);
      temp->rename(arguments[2]);
      temp->chown(user);
      subDirectories += temp;
      subDirectoryCount++;
      return;
    }//if permissions are validated
    else
        cout << "cp : Permission denied.\n"; return;
  }//if isFile


  if(!((Directory*)subDirectories[i])->permissions.check(4, user))
  {
    cout << "cp : Permission denied.\n";
    return;
  } // if permissions not OK

  Directory *temp = new Directory(*(Directory*)subDirectories[i]);
  delete temp->name;
  temp->name = new char[strlen(arguments[2]) + 1];
  strcpy(temp->name, arguments[2]);
  temp->permissions.setOwner(user);
  subDirectories += temp;
  subDirectoryCount++;
} // cp()



void Directory::ls(int argCount, const char *arguments[], const char *user)const
{
  if(argCount > 2 || (argCount == 2 && strcmp(arguments[1], "-l")))
  {
    cout << "usage: ls [-l]\n";
    return;
  } // if incorrect number of arguments

  if(subDirectoryCount == 0)
    return;

  if(!permissions.check(4, user))
  {
    cout << "ls: Permission denied.\n";
    return;
  } // if invalid permissions
  
  if(argCount == 2) // must be ls -l
  {
    for(int i = 0; i < subDirectoryCount; i++)
    {
      if(subDirectories[i]->isFile())
      {
        subDirectories[i]->print(true);
      }//if is file
      else
      {
        ((Directory*)subDirectories[i])->permissions.print();
        cout << " ";
        ((Directory*)subDirectories[i])->modificationTime.print();
        cout << ' ' << ((Directory*)subDirectories[i])->name << endl;
      }//else is directory  type
    }  // for each subdirectory
  } // if ls -l
  
  else // simple ls
  {
    for(int i = 0; i < subDirectoryCount; i++)
      if (subDirectories[i]->isFile())
      {
        subDirectories[i]->print(false);
      }//if is file
      else
        cout << ((Directory*)subDirectories[i])->name << " ";
    cout << endl;
  } // else simple ls
} // ls()

void Directory::mkdir(int argCount, const char *arguments[], const char *user)
{
  if(argCount != 2)
  {
    cout << "usage: mkdir directory_name\n";
    return;
  } // if wrong number of arguments
  
  for(int i = 0; i < subDirectoryCount; i++)
    if(strcmp(arguments[1], ((Directory*)subDirectories[i])->name) == 0)
    {
      cout << "mkdir: cannot create directory '" << arguments[1]
        << "': File exists\n";
      return;
    } // if found file with same name

  if(! permissions.check(6, user))
  {
    cout << "mkdir: Permission denied.\n";
    return;
  } // if don't have permissions 

  subDirectories += new Directory(arguments[1], this, user);
  subDirectoryCount++;
  modificationTime.update();

} // mkdir()


void Directory::showPath() const
{
  if(parent)
  {
    parent->showPath();
    cout << name << "/";
  } // if not root
  else
    cout << name;
} // showPath()

ostream& write(ostream &os)//,  Directory &directory)
{
//  Directory *directory;
//  os << 'D';
//  directory->File::write(os);
 // os << directory->subDirectoryCount;
  
//  for(int i = 0; i < directory->subDirectoryCount; i++)
 //   os << *(directory->subDirectories[i]);

  return os;
}//directory write()

istream& operator>> (istream &is, Directory &directory)
{
  char temp[80];
  is >> temp;
 
  if(directory.name)
    delete [] directory.name;

  directory.name = new char[strlen(temp) + 1];
  strcpy(directory.name, temp);
  
  is.ignore();  // eat space char
  is >> directory.modificationTime >> directory.permissions
    >> directory.subDirectoryCount;


  for(int i = 0; i < directory.subDirectoryCount; i++)
  {
    Directory *subDirectory = new Directory("Dummy");
    is >> *subDirectory;
    subDirectory->parent = &directory;
    directory.subDirectories += subDirectory;
  } // for each subdirectory

  return is;
}  // operator<<
