#include "stdafx.h"
#include "Database.h"


SQLConnect::SQLConnect()
{
	
}

bool SQLConnect::openConnection()
{
	try {
		connection->Open();
		return true;
	}
	catch (MySqlException^ ex)
	{
		switch (ex->Number)
		{
		case 0:
			MessageBox::Show("Cannot connect to database.");
			break;
		case 1:
			MessageBox::Show("Invalid username/password, please try again");
			break;
		}
	}
	return false;
}

bool SQLConnect::closeConnection()
{
	try {
		connection->Close();
		return true;
	}
	catch (MySqlException^ ex)
	{
		MessageBox::Show(ex->Message, "Error", MessageBoxButtons::OK);
		return false;
	}
}

void SQLConnect::backup(String^ path)
{
	try
	{
		DateTime Time = DateTime::Now;
		int year = Time.Year;
		int month = Time.Month;
		int day = Time.Day;
		int hour = Time.Hour;
		int minute = Time.Minute;
		int second = Time.Second;
		int millisecond = Time.Millisecond;

		//Save file to C:\ with the current date as a filename
		path = path + "\\" + year + "-" + month + "-" + day +
			"-" + hour + "-" + minute + "-" + second + "-" + millisecond + ".sql";
		StreamWriter^ file = gcnew StreamWriter(path);

		//Create our process...
		ProcessStartInfo^ psi = gcnew ProcessStartInfo();
		psi->FileName = "D:\\xampp\\mysql\\bin\\mysqldump";
		psi->RedirectStandardInput = false;
		psi->RedirectStandardOutput = true;
		psi->Arguments = String::Format(" -u {0} -p {1} -h {2} {3}", user, pass, server, database);
		psi->UseShellExecute = false;
		//...and run it
		Process^ process = Process::Start(psi);
		String^ output;
		output = process->StandardOutput->ReadToEnd();
		file->WriteLine(output);
		process->WaitForExit();
		file->Close();
		process->Close();
		MessageBox::Show("Successfully backed up!");
	}
	catch (IOException^ ex)
	{
		MessageBox::Show("Error , unable to backup!");

	}
}

bool SQLConnect::createUser(String^ user,String^ pass)
{
	try {
		openConnection();
		String^ sql;
		sql = sql->Format("Insert into users(username, pass_hash) values('{1}', AES_ENCRYPT('{2}','{3}'))",
			pass, secret, user);
		MySqlCommand^ cmd = gcnew MySqlCommand(sql, connection);
		cmd->ExecuteNonQuery();


		String^ msg;
		msg = msg->Format("New user created!");
		MessageBox::Show(msg);

	}
	catch(MySqlException^ err){
		MessageBox::Show(err->ToString());
		return false;
	}
	closeConnection();
	return true;
}



MySqlDataReader^ SQLConnect::fillEmployee() {
	MySqlDataReader^ dataset;
	try {
		openConnection();
		String^ sql;
		sql = sql->Format("SELECT first_name, last_name, password, address, wages, position FROM employee");
		MySqlCommand^ cmd = gcnew MySqlCommand(sql, connection);
		cmd->ExecuteNonQuery();
		dataset = cmd->ExecuteReader();
	}
	catch(MySqlException^ err){
		MessageBox::Show(err->ToString());
	}
	closeConnection();
	return dataset;
}

bool SQLConnect::createEmployee(String^ first_name, String^ last_name, String^ address, String^ wage, String^ position)
{
	try {
		openConnection();
		String^ sql;
		sql = sql->Format("Insert into employee(first_name, last_name, password, address, wages, position) values('{0}','{1}','{3}','{4}','{5}')",
			first_name, last_name, address, wage, position);
		MySqlCommand^ cmd = gcnew MySqlCommand(sql, connection);
		//MessageBox::Show(sql);
		cmd->ExecuteNonQuery();


		String^ msg;
		msg = msg->Format("{0} {1} has been added!", first_name, last_name);
		MessageBox::Show(msg);

	}
	catch (MySqlException^ err) {
		MessageBox::Show(err->ToString());
		return false;
	}
	closeConnection();
	return true;
}

bool SQLConnect::login(String^ user, String^ pass)
{
	try
	{
		openConnection();
		String^ sql;
		sql = sql->Format("Select username, CAST(AES_DECRYPT(password, '{0}') AS CHAR(50)) from employee where username='{1}'", secret, user);
		MySqlCommand^ cmd = gcnew MySqlCommand(sql, connection);
		MySqlDataReader^ reader = cmd->ExecuteReader();
		//MessageBox::Show(sql);

		while (reader->Read())
		{
			String^ userCheck = reader[0]->ToString();
			String^ passCheck = reader[1]->ToString();
			//MessageBox::Show(userCheck);
			//MessageBox::Show(passCheck);
			if (user == userCheck && pass == passCheck)
			{
				return true;
			}
		}
		
	}
	catch (MySqlException^ err)
	{
		MessageBox::Show(err->ToString());
		return false;
	}
	closeConnection();
	return false;
}

void SQLConnect::restore(String^ path)
{

	try
	{
		StreamReader^ file = gcnew StreamReader(path);
		String^ input = file->ReadToEnd();
		file->Close();

		//Create our process...
		ProcessStartInfo^ psi = gcnew ProcessStartInfo();
		psi->FileName = "D:\\xampp\\mysql\\bin\\mysql";
		psi->RedirectStandardInput = true;
		psi->RedirectStandardOutput = false;
		psi->Arguments = String::Format("-u {0} -p {1} -h {2} {3}", user, pass, server, database);
		psi->UseShellExecute = false;
		//...and run it
		Process^ process = Process::Start(psi);
		process->StandardInput->WriteLine(input);
		process->StandardInput->Close();
		process->WaitForExit();
		process->Close();
		MessageBox::Show("Successful restore.");
	}
	catch (IOException^ ex)
	{
		MessageBox::Show("Error , unable to Restore!");
	}
}